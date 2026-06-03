#pragma once

#include "rules/TypeCheckResult.h"
#include "src/frontend/syntax/ast/ASTNode.h"
#include "symbols/SymbolTable.h"
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

// TODO разделить на несколько классов
class SemanticAnalyzer final : public ASTNodeVisitor
{
public:
	explicit SemanticAnalyzer(SymbolTable symbols);

	[[nodiscard]] TypeCheckResult Analyze(const ASTNode& node);

	void Visit(const BoolLiteralASTNode& node) override;
	void Visit(const IntLiteralASTNode& node) override;
	void Visit(const FloatLiteralASTNode& node) override;
	void Visit(const StringLiteralASTNode& node) override;
	void Visit(const IdentifierASTNode& node) override;
	void Visit(const UnaryASTNode& node) override;
	void Visit(const BinaryASTNode& node) override;
	void Visit(const MemberAccessASTNode& node) override;
	void Visit(const IndexASTNode& node) override;
	void Visit(const CallExpressionASTNode& node) override;
	void Visit(const AssignmentASTNode& node) override;
	void Visit(const ShortVariableDeclarationASTNode& node) override;
	void Visit(const VariableDeclarationASTNode& node) override;
	void Visit(const ExpressionStatementASTNode& node) override;
	void Visit(const ProgramASTNode& node) override;
	void Visit(const StatementListASTNode& node) override;
	void Visit(const BlockASTNode& node) override;
	void Visit(const IfASTNode& node) override;
	void Visit(const PrintfASTNode& node) override;
	void Visit(const ReturnASTNode& node) override;
	void Visit(const FunctionDeclarationASTNode& node) override;

private:
	[[nodiscard]] Type AnalyzeChild(const ASTNode& node);
	[[nodiscard]] std::vector<Type> AnalyzeValues(const std::vector<ASTNodePtr>& values);
	[[nodiscard]] static bool IsFalsey(Type type);
	[[nodiscard]] static bool HasError(const std::vector<Type>& types);
	void ValidateAssignment(const std::vector<std::string>& names, const std::vector<Type>& valueTypes);
	void DefineShortVariables(const std::vector<std::string>& names, const std::vector<Type>& valueTypes);
	void DefineVariables(
		const std::vector<std::string>& names,
		std::optional<Type> declaredType,
		const std::vector<Type>& valueTypes);
	void PredeclareTopLevelFunctions(const ASTNode& node);
	void PredeclareTopLevelFunctions(const StatementListASTNode& node);
	void PredeclareFunction(const FunctionDeclarationASTNode& node);
	[[nodiscard]] bool DefineFunctionSymbol(const FunctionDeclarationASTNode& node);
	[[nodiscard]] static std::vector<Type> BuildParameterTypes(const FunctionDeclarationASTNode& node);
	[[nodiscard]] static bool AlwaysReturns(const ASTNode& node);
	[[nodiscard]] static bool StatementListAlwaysReturns(const StatementListASTNode& node);
	[[nodiscard]] static bool IfAlwaysReturns(const IfASTNode& node);
	void SetCurrentType(const ASTNode& node, Type type);
	void SetTypeCheckResult(const ASTNode& node, TypeCheckResult result);
	void AddDiagnostic(std::string message);
	void AddDiagnostics(std::vector<SemanticDiagnostic> diagnostics);

	SymbolTable m_symbolTable;
	std::optional<Type> m_currentFunctionReturnType;
	Type m_currentType = Type::ERROR;
	std::vector<SemanticDiagnostic> m_diagnostics;
	std::unordered_set<const FunctionDeclarationASTNode*> m_predeclaredFunctions;
};
