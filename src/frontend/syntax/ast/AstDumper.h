#pragma once

#include "ASTNode.h"
#include <iosfwd>

class AstDumper final : public ASTNodeVisitor
{
public:
	static void Dump(const ASTNode& expr, std::ostream& output);
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

private:
	explicit AstDumper(std::ostream& output);

	void DumpChild(const ASTNode& expr);
	void DumpLine(const std::string& text) const;
	static std::string JoinNames(const std::vector<std::string>& names);
	static std::string JoinParameters(const std::vector<FunctionParameter>& parameters);
	static std::string ToString(const TypeDescriptor& type);
	static const char* ToString(UnaryOperator op);
	static const char* ToString(BinaryOperator op);

	std::ostream& m_output;
	int m_indent = 0;
};
