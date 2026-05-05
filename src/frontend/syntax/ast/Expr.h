#pragma once

#include <memory>
#include <string>

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

struct Expr
{
	virtual ~Expr() = default;
};

using ExprPtr = std::unique_ptr<Expr>;

struct BoolLiteralExpr final : Expr
{
	explicit BoolLiteralExpr(const bool value)
		: value(value)
	{
	}

	bool value;
};

struct IntLiteralExpr final : Expr
{
	explicit IntLiteralExpr(std::string value)
		: value(std::move(value))
	{
	}

	std::string value;
};

struct FloatLiteralExpr final : Expr
{
	explicit FloatLiteralExpr(std::string value)
		: value(std::move(value))
	{
	}

	std::string value;
};

struct IdentifierExpr final : Expr
{
	explicit IdentifierExpr(std::string name)
		: name(std::move(name))
	{
	}

	std::string name;
};

struct UnaryExpr final : Expr
{
	UnaryExpr(const UnaryOperator op, ExprPtr operand)
		: op(op)
		, operand(std::move(operand))
	{
	}

	UnaryOperator op;
	ExprPtr operand;
};

struct BinaryExpr final : Expr
{
	BinaryExpr(ExprPtr left, const BinaryOperator op, ExprPtr right)
		: left(std::move(left))
		, op(op)
		, right(std::move(right))
	{
	}

	ExprPtr left;
	BinaryOperator op;
	ExprPtr right;
};

struct MemberAccessExpr final : Expr
{
	MemberAccessExpr(ExprPtr object, std::string member)
		: object(std::move(object))
		, member(std::move(member))
	{
	}

	ExprPtr object;
	std::string member;
};

struct IndexExpr final : Expr
{
	IndexExpr(ExprPtr object, ExprPtr index)
		: object(std::move(object))
		, index(std::move(index))
	{
	}

	ExprPtr object;
	ExprPtr index;
};
