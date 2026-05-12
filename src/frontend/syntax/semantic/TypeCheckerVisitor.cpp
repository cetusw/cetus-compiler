#include "TypeCheckerVisitor.h"

#include "TypeRules.h"
#include "src/frontend/syntax/ast/Expr.h"

TypeCheckerVisitor::TypeCheckerVisitor(SymbolTable symbols)
	: m_symbols(std::move(symbols))
{
}

TypeCheckResult TypeCheckerVisitor::Check(const Expr& expr)
{
	m_currentType = Type::ERROR;
	m_error.reset();
	m_nodeTypes.clear();
	expr.Accept(*this);
	if (m_error.has_value())
	{
		return TypeCheckResult::Error(*m_error, std::move(m_nodeTypes));
	}

	return TypeCheckResult::Success(m_currentType, std::move(m_nodeTypes));
}

void TypeCheckerVisitor::Visit(const BoolLiteralExpr& expr)
{
	SetCurrentType(expr, Type::BOOL);
}

void TypeCheckerVisitor::Visit(const IntLiteralExpr& expr)
{
	SetCurrentType(expr, Type::INT);
}

void TypeCheckerVisitor::Visit(const FloatLiteralExpr& expr)
{
	SetCurrentType(expr, Type::FLOAT);
}

void TypeCheckerVisitor::Visit(const IdentifierExpr& expr)
{
	const Symbol* symbol = m_symbols.Resolve(expr.GetName());
	if (!symbol)
	{
		Fail("Undefined identifier: " + expr.GetName());
		return;
	}

	SetCurrentType(expr, symbol->type);
}

void TypeCheckerVisitor::Visit(const UnaryExpr& expr)
{
	const Type operandType = CheckChild(expr.GetOperand());
	if (m_error.has_value())
	{
		return;
	}

	SetTypeCheckResult(expr, TypeRules::CheckUnaryOperator(expr.GetOperator(), operandType));
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

	SetTypeCheckResult(expr, TypeRules::CheckBinaryOperator(expr.GetOperator(), leftType, rightType));
}

void TypeCheckerVisitor::Visit(const MemberAccessExpr&)
{
	Fail("Member access type checking is not implemented yet.");
}

void TypeCheckerVisitor::Visit(const IndexExpr&)
{
	Fail("Index access type checking is not implemented yet.");
}

void TypeCheckerVisitor::Visit(const AssignmentExpr& expr)
{
	const Type valueType = CheckChild(expr.GetValue());
	if (m_error.has_value())
	{
		return;
	}

	const Symbol* existing = m_symbols.Resolve(expr.GetName());
	if (existing && existing->type != valueType)
	{
		Fail("Cannot assign value of different type to identifier: " + expr.GetName());
		return;
	}
	if (!existing)
	{
		m_symbols.Define(Symbol{ expr.GetName(), valueType });
	}

	SetCurrentType(expr, valueType);
}

void TypeCheckerVisitor::Visit(const SequenceExpr& expr)
{
	if (expr.GetExpressions().empty())
	{
		Fail("Sequence expression cannot be empty.");
		return;
	}

	Type lastType = Type::ERROR;
	for (const ExprPtr& child : expr.GetExpressions())
	{
		lastType = CheckChild(*child);
		if (m_error.has_value())
		{
			return;
		}
	}

	SetCurrentType(expr, lastType);
}

Type TypeCheckerVisitor::CheckChild(const Expr& expr)
{
	expr.Accept(*this);
	return m_currentType;
}

void TypeCheckerVisitor::SetCurrentType(const Expr& expr, const Type type)
{
	m_currentType = type;
	m_nodeTypes[&expr] = type;
}

void TypeCheckerVisitor::SetTypeCheckResult(const Expr& expr, TypeCheckResult result)
{
	if (result.error.has_value())
	{
		Fail(std::move(*result.error));
		return;
	}

	SetCurrentType(expr, result.type);
}

void TypeCheckerVisitor::Fail(std::string message)
{
	m_currentType = Type::ERROR;
	m_error = std::move(message);
}
