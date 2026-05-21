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
	void Visit(const IdentifierASTNode& expr) override;
	void Visit(const UnaryASTNode& expr) override;
	void Visit(const BinaryASTNode& expr) override;
	void Visit(const MemberAccessASTNode& expr) override;
	void Visit(const IndexASTNode& expr) override;
	void Visit(const AssignmentASTNode& expr) override;
	void Visit(const SequenceASTNode& expr) override;
	void Visit(const IfElseASTNode& expr) override;
	void Visit(const PrintfASTNode& expr) override;

private:
	explicit AstDumper(std::ostream& output);

	void DumpChild(const ASTNode& expr);
	void DumpLine(const std::string& text) const;
	static const char* ToString(UnaryOperator op);
	static const char* ToString(BinaryOperator op);

	std::ostream& m_output;
	int m_indent = 0;
};
