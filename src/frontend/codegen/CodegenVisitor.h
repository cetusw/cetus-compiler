#pragma once

#include "../semantic/rules/TypeCheckResult.h"
#include "../semantic/symbols/SymbolTable.h"
#include "CodegenResult.h"
#include "FunctionContext.h"
#include "ProgramContext.h"
#include "src/frontend/syntax/ast/ASTNode.h"
#include <vector>

class ASTNode;

class CodegenVisitor final : public ASTNodeVisitor
{
public:
	CodegenVisitor(const SymbolTable& symbols, const TypeCheckResult& typeInfo);

	[[nodiscard]] CodegenResult Generate(const ASTNode& expr);

	void Visit(const BoolLiteralASTNode& expr) override;
	void Visit(const IntLiteralASTNode& expr) override;
	void Visit(const FloatLiteralASTNode& expr) override;
	void Visit(const StringLiteralASTNode& expr) override;
	void Visit(const IdentifierASTNode& expr) override;
	void Visit(const AddressOfASTNode& expr) override;
	void Visit(const UnaryASTNode& expr) override;
	void Visit(const BinaryASTNode& expr) override;
	void Visit(const MemberAccessASTNode& expr) override;
	void Visit(const IndexASTNode& expr) override;
	void Visit(const CallExpressionASTNode& expr) override;
	void Visit(const AssignmentASTNode& expr) override;
	void Visit(const ShortVariableDeclarationASTNode& expr) override;
	void Visit(const VariableDeclarationASTNode& expr) override;
	void Visit(const ExpressionStatementASTNode& expr) override;
	void Visit(const ProgramASTNode& expr) override;
	void Visit(const StatementListASTNode& expr) override;
	void Visit(const BlockASTNode& expr) override;
	void Visit(const IfASTNode& expr) override;
	void Visit(const ForASTNode& expr) override;
	void Visit(const BreakASTNode& expr) override;
	void Visit(const ContinueASTNode& expr) override;
	void Visit(const ReturnASTNode& expr) override;
	void Visit(const FunctionDeclarationASTNode& expr) override;
	void Visit(const StructDeclarationASTNode& expr) override;

private:
	[[nodiscard]] BytecodeEmitter& CurrentEmitter();
	void Fail(std::string message);
	void EmitDefault(const TypeDescriptor& type);
	void EmitStructDefault(const TypeDescriptor& type);
	void EmitScalarDefault(Type type);
	void EmitAssignmentTarget(const ASTNode& target);
	void EmitBinaryOperation(BinaryOperator op);
	void EmitLogicalAnd(const BinaryASTNode& expr);
	void EmitLogicalOr(const BinaryASTNode& expr);
	void EmitScopeCleanup(int scopeDepth);
	void EmitIdentifierRef(const IdentifierASTNode& expr);
	[[nodiscard]] bool ShouldPassArgumentByPointer(const std::string& calleeName, std::size_t argumentIndex) const;
	[[nodiscard]] bool EnsureTyped(const ASTNode& expr);

	struct LoopContext
	{
		std::vector<int> breakJumps;
		std::vector<int> continueJumps;
		int breakScopeDepth = 0;
		int continueScopeDepth = 0;
		int continueTarget = 0;
		bool continueJumpsForward = false;
	};

	const SymbolTable& m_symbols;
	const TypeCheckResult& m_typeInfo;
	ProgramContext m_programContext;
	std::vector<FunctionContext> m_functionStack;
	std::vector<LoopContext> m_loopStack;
	std::optional<std::string> m_error;
};
