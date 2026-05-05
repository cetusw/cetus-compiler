#include "AstDumper.h"

#include <ostream>
#include <stdexcept>

void AstDumper::Dump(const Expr& expr, std::ostream& output)
{
	AstDumper dumper(output);
	expr.Accept(dumper);
}

AstDumper::AstDumper(std::ostream& output)
	: m_output(output)
{
}

void AstDumper::Visit(const BoolLiteralExpr& expr)
{
	DumpLine(std::string("BoolLiteralExpr(") + (expr.GetValue() ? "true" : "false") + ")");
}

void AstDumper::Visit(const IntLiteralExpr& expr)
{
	DumpLine("IntLiteralExpr(" + expr.GetValue() + ")");
}

void AstDumper::Visit(const FloatLiteralExpr& expr)
{
	DumpLine("FloatLiteralExpr(" + expr.GetValue() + ")");
}

void AstDumper::Visit(const IdentifierExpr& expr)
{
	DumpLine("IdentifierExpr(" + expr.GetName() + ")");
}

void AstDumper::Visit(const UnaryExpr& expr)
{
	DumpLine(std::string("UnaryExpr(") + ToString(expr.GetOperator()) + ")");
	DumpChild(expr.GetOperand());
}

void AstDumper::Visit(const BinaryExpr& expr)
{
	DumpLine(std::string("BinaryExpr(") + ToString(expr.GetOperator()) + ")");
	DumpChild(expr.GetLeft());
	DumpChild(expr.GetRight());
}

void AstDumper::Visit(const MemberAccessExpr& expr)
{
	DumpLine("MemberAccessExpr(" + expr.GetMember() + ")");
	DumpChild(expr.GetObject());
}

void AstDumper::Visit(const IndexExpr& expr)
{
	DumpLine("IndexExpr");
	DumpChild(expr.GetObject());
	DumpChild(expr.GetIndex());
}

void AstDumper::DumpChild(const Expr& expr)
{
	m_indent += 2;
	expr.Accept(*this);
	m_indent -= 2;
}

void AstDumper::DumpLine(const std::string& text) const
{
	m_output << std::string(m_indent, ' ') << text << '\n';
}

const char* AstDumper::ToString(const UnaryOperator op)
{
	switch (op)
	{
	case UnaryOperator::NEGATE: return "NEGATE";
	case UnaryOperator::NOT: return "NOT";
	}

	throw std::runtime_error("Unsupported unary operator.");
}

const char* AstDumper::ToString(const BinaryOperator op)
{
	switch (op)
	{
	case BinaryOperator::OR: return "OR";
	case BinaryOperator::AND: return "AND";
	case BinaryOperator::ADD: return "ADD";
	case BinaryOperator::SUBTRACT: return "SUBTRACT";
	case BinaryOperator::MULTIPLY: return "MULTIPLY";
	case BinaryOperator::DIVIDE: return "DIVIDE";
	case BinaryOperator::MODULO: return "MODULO";
	case BinaryOperator::LESS: return "LESS";
	case BinaryOperator::LESS_EQUAL: return "LESS_EQUAL";
	case BinaryOperator::NOT_EQUAL: return "NOT_EQUAL";
	case BinaryOperator::EQUAL: return "EQUAL";
	case BinaryOperator::GREATER: return "GREATER";
	case BinaryOperator::GREATER_EQUAL: return "GREATER_EQUAL";
	}

	throw std::runtime_error("Unsupported binary operator.");
}
