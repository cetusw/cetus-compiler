#pragma once

#include "rules/TypeCheckResult.h"
#include "src/frontend/syntax/ast/ASTNode.h"
#include "symbols/SymbolTable.h"
#include <string>

class SemanticAnalyzer final : public ASTNodeVisitor
{
public:
	explicit SemanticAnalyzer(SymbolTable symbols);

	[[nodiscard]] TypeCheckResult Analyze(const ASTNode& node);

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

private:
	[[nodiscard]] Type AnalyzeChild(const ASTNode& node);
	void SetCurrentType(const ASTNode& node, Type type);
	void SetTypeCheckResult(const ASTNode& node, TypeCheckResult result);
	void AddDiagnostic(std::string message);
	void AddDiagnostics(std::vector<SemanticDiagnostic> diagnostics);

	SymbolTable m_symbolTable;
	Type m_currentType = Type::ERROR;
	std::vector<SemanticDiagnostic> m_diagnostics;
};
