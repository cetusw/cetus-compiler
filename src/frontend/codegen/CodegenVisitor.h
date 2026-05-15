#pragma once

#include "../semantic/rules/TypeCheckResult.h"
#include "../semantic/symbols/SymbolTable.h"
#include "CodegenResult.h"
#include "src/backend/vm/types/OpCode.h"
#include "src/frontend/syntax/ast/ASTNode.h"

class Chunk;
class ASTNode;
class ObjFunction;

class CodegenVisitor final : public ASTNodeVisitor
{
public:
	CodegenVisitor(const SymbolTable& symbols, const TypeCheckResult& typeInfo);

	[[nodiscard]] CodegenResult Generate(const ASTNode& expr);

	void Visit(const BoolLiteralASTNode& expr) override;
	void Visit(const IntLiteralASTNode& expr) override;
	void Visit(const FloatLiteralASTNode& expr) override;
	void Visit(const IdentifierASTNode& expr) override;
	void Visit(const UnaryASTNode& expr) override;
	void Visit(const BinaryASTNode& expr) override;
	void Visit(const MemberAccessASTNode& expr) override;
	void Visit(const IndexASTNode& expr) override;
	void Visit(const AssignmentASTNode& expr) override;
	void Visit(const SequenceASTNode& expr) override;

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
	void EmitLogicalAnd(const BinaryASTNode& expr);
	void EmitLogicalOr(const BinaryASTNode& expr);
	void EmitGlobalLoad(const std::string& name);
	[[nodiscard]] bool EnsureTyped(const ASTNode& expr);

	const SymbolTable& m_symbols;
	const TypeCheckResult& m_typeInfo;
	std::shared_ptr<ObjFunction> m_function;
	std::optional<std::string> m_error;
};
