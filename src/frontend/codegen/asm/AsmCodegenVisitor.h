#pragma once

#include "../../semantic/rules/TypeCheckResult.h"
#include "AsmProgram.h"
#include "AsmCodegenResult.h"
#include "src/frontend/syntax/ast/ASTNode.h"

#include <optional>
#include <string>
#include <unordered_set>

class AsmCodegenVisitor : public ASTNodeVisitor
{
public:
	explicit AsmCodegenVisitor(const TypeCheckResult& typeInfo);

	[[nodiscard]] AsmCodegenResult Generate(const ASTNode& node);

	void Visit(const BoolLiteralASTNode& node) override;
	void Visit(const IntLiteralASTNode& node) override;
	void Visit(const FloatLiteralASTNode& node) override;
	void Visit(const IdentifierASTNode& node) override;
	void Visit(const UnaryASTNode& node) override;
	void Visit(const BinaryASTNode& node) override;
	void Visit(const MemberAccessASTNode& node) override;
	void Visit(const IndexASTNode& node) override;
	void Visit(const AssignmentASTNode& node) override;
	void Visit(const SequenceASTNode& node) override;
	void Visit(const IfASTNode& node) override;
	void Visit(const PrintfASTNode& node) override;

private:
	void Emit(std::string line);
	void EmitProgramPreamble();
	void EmitProgramPostamble();
	void EmitLoadVariable(const std::string& name);
	void EmitStoreVariable(const std::string& name);
	void EmitUnaryNegation(const UnaryASTNode& node);
	void EmitLogicalNot(const UnaryASTNode& node);
	[[nodiscard]] bool EmitShortCircuitIfNeeded(const BinaryASTNode& node);
	void EmitBinaryOperands(const BinaryASTNode& node);
	void EmitBinaryOperation(BinaryOperator op);
	void EmitSimpleArithmetic(BinaryOperator op);
	void EmitDivision();
	void EmitModulo();
	void EmitBinaryComparison(BinaryOperator op);
	void EmitLogicalAnd(const BinaryASTNode& node);
	void EmitLogicalOr(const BinaryASTNode& node);
	void EmitPrintf();
	void EnsureVariableDeclared(const std::string& name);
	[[nodiscard]] std::string NextLabel(const std::string& prefix);
	void Fail(std::string message);
	[[nodiscard]] bool EnsureTyped(const ASTNode& node);
	[[nodiscard]] bool EnsureSupportedType(const ASTNode& node);
	[[nodiscard]] static bool IsIntTyped(const ASTNode& node);
	[[nodiscard]] static bool IsBoolTyped(const ASTNode& node);
	[[nodiscard]] static bool IsFalsey(const ASTNode& node);
	[[nodiscard]] static bool IsSupportedBinary(BinaryOperator op);
	[[nodiscard]] static bool IsSimpleArithmetic(BinaryOperator op);
	[[nodiscard]] static bool IsComparison(BinaryOperator op);

	const TypeCheckResult& m_typeInfo;
	AsmProgram m_program;
	std::unordered_set<std::string> m_declaredVariables;
	std::optional<std::string> m_error;
	int m_nextLabelId = 0;
};
