#pragma once

#include "Expr.h"
#include <iosfwd>

class AstDumper final : public ExprVisitor
{
public:
	static void Dump(const Expr& expr, std::ostream& output);
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
	explicit AstDumper(std::ostream& output);

	void DumpChild(const Expr& expr);
	void DumpLine(const std::string& text) const;
	static const char* ToString(UnaryOperator op);
	static const char* ToString(BinaryOperator op);

	std::ostream& m_output;
	int m_indent = 0;
};
