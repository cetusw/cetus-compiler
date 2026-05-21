#include "AstDumper.h"

#include <ostream>
#include <stdexcept>

void AstDumper::Dump(const ASTNode& expr, std::ostream& output)
{
	AstDumper dumper(output);
	expr.Accept(dumper);
}

AstDumper::AstDumper(std::ostream& output)
	: m_output(output)
{
}

void AstDumper::Visit(const BoolLiteralASTNode& expr)
{
	DumpLine(std::string("BoolLiteralASTNode(") + (expr.GetValue() ? "true" : "false") + ")");
}

void AstDumper::Visit(const IntLiteralASTNode& expr)
{
	DumpLine("IntLiteralASTNode(" + expr.GetValue() + ")");
}

void AstDumper::Visit(const FloatLiteralASTNode& expr)
{
	DumpLine("FloatLiteralASTNode(" + expr.GetValue() + ")");
}

void AstDumper::Visit(const IdentifierASTNode& expr)
{
	DumpLine("IdentifierASTNode(" + expr.GetName() + ")");
}

void AstDumper::Visit(const UnaryASTNode& expr)
{
	DumpLine(std::string("UnaryASTNode(") + ToString(expr.GetOperator()) + ")");
	DumpChild(expr.GetOperand());
}

void AstDumper::Visit(const BinaryASTNode& expr)
{
	DumpLine(std::string("BinaryASTNode(") + ToString(expr.GetOperator()) + ")");
	DumpChild(expr.GetLeft());
	DumpChild(expr.GetRight());
}

void AstDumper::Visit(const MemberAccessASTNode& expr)
{
	DumpLine("MemberAccessASTNode(" + expr.GetMember() + ")");
	DumpChild(expr.GetObject());
}

void AstDumper::Visit(const IndexASTNode& expr)
{
	DumpLine("IndexASTNode");
	DumpChild(expr.GetObject());
	DumpChild(expr.GetIndex());
}

void AstDumper::Visit(const AssignmentASTNode& expr)
{
	DumpLine("AssignmentASTNode(" + expr.GetName() + ")");
	DumpChild(expr.GetValue());
}

void AstDumper::Visit(const SequenceASTNode& expr)
{
	DumpLine("SequenceASTNode");
	for (const ASTNodePtr& child : expr.GetExpressions())
	{
		DumpChild(*child);
	}
}

void AstDumper::Visit(const IfElseASTNode& expr)
{
	DumpLine("IfElseASTNode");
	DumpChild(expr.GetCondition());
	DumpChild(expr.GetThenBranch());
	DumpChild(expr.GetElseBranch());
}

void AstDumper::Visit(const PrintfASTNode& expr)
{
	DumpLine("PrintfASTNode");
	DumpChild(expr.GetArgument());
}

void AstDumper::DumpChild(const ASTNode& expr)
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
