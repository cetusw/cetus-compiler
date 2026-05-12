#pragma once

#include "AsmProgram.h"
#include "AsmCodegenResult.h"
#include "src/frontend/syntax/ast/Expr.h"
#include "src/frontend/syntax/semantic/TypeCheckResult.h"

#include <optional>
#include <string>
#include <unordered_set>

class AsmCodegenVisitor : public ExprVisitor
{
public:
	explicit AsmCodegenVisitor(const TypeCheckResult& typeInfo);

	[[nodiscard]] AsmCodegenResult Generate(const Expr& expr);

	void Visit(const BoolLiteralExpr& expr) override;
	void Visit(const IntLiteralExpr& expr) override;
	void Visit(const FloatLiteralExpr& expr) override;
	void Visit(const IdentifierExpr& expr) override;
	void Visit(const UnaryExpr& expr) override;
	void Visit(const BinaryExpr& expr) override;
	void Visit(const MemberAccessExpr& expr) override;
	void Visit(const IndexExpr& expr) override;
	void Visit(const AssignmentExpr& expr) override;
	void Visit(const SequenceExpr& expr) override;

private:
	void Emit(std::string line);
	void EmitProgramPreamble();
	void EmitProgramPostamble();
	void EmitBinaryComparison(BinaryOperator op);
	void EmitLogicalAnd(const BinaryExpr& expr);
	void EmitLogicalOr(const BinaryExpr& expr);
	void EnsureVariableDeclared(const std::string& name);
	[[nodiscard]] std::string NextLabel(std::string prefix);
	void Fail(std::string message);
	[[nodiscard]] bool EnsureTyped(const Expr& expr);
	[[nodiscard]] bool EnsureSupportedType(const Expr& expr);
	[[nodiscard]] bool IsIntTyped(const Expr& expr) const;
	[[nodiscard]] bool IsBoolTyped(const Expr& expr) const;
	[[nodiscard]] static bool IsSupportedBinary(BinaryOperator op);

	const TypeCheckResult& m_typeInfo;
	AsmProgram m_program;
	std::unordered_set<std::string> m_declaredVariables;
	std::optional<std::string> m_error;
	int m_nextLabelId = 0;
};
