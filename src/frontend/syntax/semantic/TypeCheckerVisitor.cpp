#include "TypeCheckerVisitor.h"

#include "TypeRules.h"
#include "src/frontend/syntax/ast/Expr.h"

TypeCheckerVisitor::TypeCheckerVisitor(TypeEnvironment environment)
	: m_environment(std::move(environment))
{
}

TypeCheckResult TypeCheckerVisitor::Check(const Expr& expr)
{
	m_currentType = Type::ERROR;
	m_error.reset();
	expr.Accept(*this);
	return { m_currentType, m_error };
}

void TypeCheckerVisitor::Visit(const BoolLiteralExpr&)
{
	m_currentType = Type::BOOL;
}

void TypeCheckerVisitor::Visit(const IntLiteralExpr&)
{
	m_currentType = Type::INT;
}

void TypeCheckerVisitor::Visit(const FloatLiteralExpr&)
{
	m_currentType = Type::FLOAT;
}

void TypeCheckerVisitor::Visit(const IdentifierExpr& expr)
{
	const std::optional<Type> type = m_environment.Resolve(expr.GetName());
	if (!type.has_value())
	{
		Fail("Undefined identifier: " + expr.GetName());
		return;
	}

	m_currentType = *type;
}

void TypeCheckerVisitor::Visit(const UnaryExpr& expr)
{
	const Type operandType = CheckChild(expr.GetOperand());
	if (m_error.has_value())
	{
		return;
	}

	SetTypeCheckResult(TypeRules::CheckUnaryOperator(expr.GetOperator(), operandType));
}

void TypeCheckerVisitor::Visit(const BinaryExpr& expr)
{
	const Type leftType = CheckChild(expr.GetLeft());
	if (m_error.has_value())
	{
		return;
	}

	const Type rightType = CheckChild(expr.GetRight());
	if (m_error.has_value())
	{
		return;
	}

	SetTypeCheckResult(TypeRules::CheckBinaryOperator(expr.GetOperator(), leftType, rightType));
}

void TypeCheckerVisitor::Visit(const MemberAccessExpr&)
{
	Fail("Member access type checking is not implemented yet.");
}

void TypeCheckerVisitor::Visit(const IndexExpr&)
{
	Fail("Index access type checking is not implemented yet.");
}

Type TypeCheckerVisitor::CheckChild(const Expr& expr)
{
	expr.Accept(*this);
	return m_currentType;
}

void TypeCheckerVisitor::SetTypeCheckResult(TypeCheckResult result)
{
	if (result.error.has_value())
	{
		Fail(std::move(*result.error));
		return;
	}

	m_currentType = result.type;
}

void TypeCheckerVisitor::Fail(std::string message)
{
	m_currentType = Type::ERROR;
	m_error = std::move(message);
}
