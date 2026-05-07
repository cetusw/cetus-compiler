#pragma once

#include "CodegenResult.h"
#include "src/backend/vm/types/OpCode.h"
#include "src/frontend/syntax/ast/Expr.h"
#include "src/frontend/syntax/semantic/SymbolTable.h"
#include "src/frontend/syntax/semantic/TypeCheckResult.h"

class Chunk;
class Expr;
class ObjFunction;

class CodegenVisitor final : public ExprVisitor
{
public:
	CodegenVisitor(const SymbolTable& symbols, const TypeCheckResult& typeInfo);

	[[nodiscard]] CodegenResult Generate(const Expr& expr);

	void Visit(const BoolLiteralExpr& expr) override;
	void Visit(const IntLiteralExpr& expr) override;
	void Visit(const FloatLiteralExpr& expr) override;
	void Visit(const IdentifierExpr& expr) override;
	void Visit(const UnaryExpr& expr) override;
	void Visit(const BinaryExpr& expr) override;
	void Visit(const MemberAccessExpr& expr) override;
	void Visit(const IndexExpr& expr) override;

private:
	static constexpr int DEFAULT_LINE = 1;

	[[nodiscard]] Chunk& CurrentChunk() const;
	void Fail(std::string message);
	void EmitByte(uint8_t byte) const;
	void EmitOpcode(OpCode opcode) const;
	void EmitOperandByte(int value);
	void EmitConstant(const Value& value);
	[[nodiscard]] int EmitJump(OpCode opcode) const;
	void PatchJump(int jumpOffset);
	void EmitShortOperand(int value, int patchOffset) const;
	void EmitBinaryOperation(BinaryOperator op);
	void EmitLogicalAnd(const BinaryExpr& expr);
	void EmitLogicalOr(const BinaryExpr& expr);
	void EmitGlobalLoad(const std::string& name);
	[[nodiscard]] bool EnsureTyped(const Expr& expr);

	const SymbolTable& m_symbols;
	const TypeCheckResult& m_typeInfo;
	std::shared_ptr<ObjFunction> m_function;
	std::optional<std::string> m_error;
};
