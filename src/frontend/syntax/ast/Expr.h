#pragma once

#include "ExprVisitor.h"
#include <memory>
#include <string>
#include <vector>

enum class UnaryOperator
{
	NEGATE,
	NOT
};

enum class BinaryOperator
{
	OR,
	AND,
	ADD,
	SUBTRACT,
	MULTIPLY,
	DIVIDE,
	MODULO,
	LESS,
	LESS_EQUAL,
	NOT_EQUAL,
	EQUAL,
	GREATER,
	GREATER_EQUAL
};

class Expr
{
public:
	virtual ~Expr() = default;
	virtual void Accept(ExprVisitor& visitor) const = 0;
};

using ExprPtr = std::unique_ptr<Expr>;

class BoolLiteralExpr final : public Expr
{
public:
	explicit BoolLiteralExpr(bool value);

	[[nodiscard]] bool GetValue() const;
	void Accept(ExprVisitor& visitor) const override;

private:
	bool m_value;
};

class IntLiteralExpr final : public Expr
{
public:
	explicit IntLiteralExpr(std::string value);

	[[nodiscard]] const std::string& GetValue() const;
	void Accept(ExprVisitor& visitor) const override;

private:
	std::string m_value;
};

class FloatLiteralExpr final : public Expr
{
public:
	explicit FloatLiteralExpr(std::string value);

	[[nodiscard]] const std::string& GetValue() const;
	void Accept(ExprVisitor& visitor) const override;

private:
	std::string m_value;
};

class IdentifierExpr final : public Expr
{
public:
	explicit IdentifierExpr(std::string name);

	[[nodiscard]] const std::string& GetName() const;
	void Accept(ExprVisitor& visitor) const override;

private:
	std::string m_name;
};

class UnaryExpr final : public Expr
{
public:
	UnaryExpr(UnaryOperator op, ExprPtr operand);

	[[nodiscard]] UnaryOperator GetOperator() const;
	[[nodiscard]] const Expr& GetOperand() const;
	void Accept(ExprVisitor& visitor) const override;

private:
	UnaryOperator m_op;
	ExprPtr m_operand;
};

class BinaryExpr final : public Expr
{
public:
	BinaryExpr(ExprPtr left, BinaryOperator op, ExprPtr right);

	[[nodiscard]] const Expr& GetLeft() const;
	[[nodiscard]] BinaryOperator GetOperator() const;
	[[nodiscard]] const Expr& GetRight() const;
	void Accept(ExprVisitor& visitor) const override;

private:
	ExprPtr m_left;
	BinaryOperator m_op;
	ExprPtr m_right;
};

class MemberAccessExpr final : public Expr
{
public:
	MemberAccessExpr(ExprPtr object, std::string member);

	[[nodiscard]] const Expr& GetObject() const;
	[[nodiscard]] const std::string& GetMember() const;
	void Accept(ExprVisitor& visitor) const override;

private:
	ExprPtr m_object;
	std::string m_member;
};

class IndexExpr final : public Expr
{
public:
	IndexExpr(ExprPtr object, ExprPtr index);

	[[nodiscard]] const Expr& GetObject() const;
	[[nodiscard]] const Expr& GetIndex() const;
	void Accept(ExprVisitor& visitor) const override;

private:
	ExprPtr m_object;
	ExprPtr m_index;
};

class AssignmentExpr final : public Expr
{
public:
	AssignmentExpr(std::string name, ExprPtr value);

	[[nodiscard]] const std::string& GetName() const;
	[[nodiscard]] const Expr& GetValue() const;
	void Accept(ExprVisitor& visitor) const override;

private:
	std::string m_name;
	ExprPtr m_value;
};

class SequenceExpr final : public Expr
{
public:
	explicit SequenceExpr(std::vector<ExprPtr> expressions);

	[[nodiscard]] const std::vector<ExprPtr>& GetExpressions() const;
	void Accept(ExprVisitor& visitor) const override;

private:
	std::vector<ExprPtr> m_expressions;
};
