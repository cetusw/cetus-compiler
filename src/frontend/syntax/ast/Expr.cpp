#include "Expr.h"

#include <utility>

BoolLiteralExpr::BoolLiteralExpr(const bool value)
	: m_value(value)
{
}

bool BoolLiteralExpr::GetValue() const
{
	return m_value;
}

void BoolLiteralExpr::Accept(ExprVisitor& visitor) const
{
	visitor.Visit(*this);
}

IntLiteralExpr::IntLiteralExpr(std::string value)
	: m_value(std::move(value))
{
}

const std::string& IntLiteralExpr::GetValue() const
{
	return m_value;
}

void IntLiteralExpr::Accept(ExprVisitor& visitor) const
{
	visitor.Visit(*this);
}

FloatLiteralExpr::FloatLiteralExpr(std::string value)
	: m_value(std::move(value))
{
}

const std::string& FloatLiteralExpr::GetValue() const
{
	return m_value;
}

void FloatLiteralExpr::Accept(ExprVisitor& visitor) const
{
	visitor.Visit(*this);
}

IdentifierExpr::IdentifierExpr(std::string name)
	: m_name(std::move(name))
{
}

const std::string& IdentifierExpr::GetName() const
{
	return m_name;
}

void IdentifierExpr::Accept(ExprVisitor& visitor) const
{
	visitor.Visit(*this);
}

UnaryExpr::UnaryExpr(const UnaryOperator op, ExprPtr operand)
	: m_op(op)
	, m_operand(std::move(operand))
{
}

UnaryOperator UnaryExpr::GetOperator() const
{
	return m_op;
}

const Expr& UnaryExpr::GetOperand() const
{
	return *m_operand;
}

void UnaryExpr::Accept(ExprVisitor& visitor) const
{
	visitor.Visit(*this);
}

BinaryExpr::BinaryExpr(ExprPtr left, const BinaryOperator op, ExprPtr right)
	: m_left(std::move(left))
	, m_op(op)
	, m_right(std::move(right))
{
}

const Expr& BinaryExpr::GetLeft() const
{
	return *m_left;
}

BinaryOperator BinaryExpr::GetOperator() const
{
	return m_op;
}

const Expr& BinaryExpr::GetRight() const
{
	return *m_right;
}

void BinaryExpr::Accept(ExprVisitor& visitor) const
{
	visitor.Visit(*this);
}

MemberAccessExpr::MemberAccessExpr(ExprPtr object, std::string member)
	: m_object(std::move(object))
	, m_member(std::move(member))
{
}

const Expr& MemberAccessExpr::GetObject() const
{
	return *m_object;
}

const std::string& MemberAccessExpr::GetMember() const
{
	return m_member;
}

void MemberAccessExpr::Accept(ExprVisitor& visitor) const
{
	visitor.Visit(*this);
}

IndexExpr::IndexExpr(ExprPtr object, ExprPtr index)
	: m_object(std::move(object))
	, m_index(std::move(index))
{
}

const Expr& IndexExpr::GetObject() const
{
	return *m_object;
}

const Expr& IndexExpr::GetIndex() const
{
	return *m_index;
}

void IndexExpr::Accept(ExprVisitor& visitor) const
{
	visitor.Visit(*this);
}
