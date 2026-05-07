#include "TypeRules.h"

TypeCheckResult TypeRules::CheckUnaryOperator(const UnaryOperator op, const Type operandType)
{
	switch (op)
	{
	case UnaryOperator::NEGATE:
		if (!IsNumeric(operandType))
		{
			return MakeError(std::string("Unary '-' expects numeric operand, got ") + ToString(operandType));
		}
		return TypeCheckResult::Success(operandType, {});
	case UnaryOperator::NOT:
		if (operandType != Type::BOOL)
		{
			return MakeError(std::string("Unary '!' expects bool operand, got ") + ToString(operandType));
		}
		return TypeCheckResult::Success(Type::BOOL, {});
	}

	return MakeError("Unsupported unary operator.");
}

TypeCheckResult TypeRules::CheckBinaryOperator(
	const BinaryOperator op,
	const Type leftType,
	const Type rightType)
{
	switch (op)
	{
	case BinaryOperator::OR:
	case BinaryOperator::AND:
		return CheckLogicalOperator(leftType, rightType);
	case BinaryOperator::ADD:
	case BinaryOperator::SUBTRACT:
	case BinaryOperator::MULTIPLY:
	case BinaryOperator::DIVIDE:
		return CheckArithmeticOperator(leftType, rightType);
	case BinaryOperator::MODULO:
		return CheckModuloOperator(leftType, rightType);
	case BinaryOperator::LESS:
	case BinaryOperator::LESS_EQUAL:
	case BinaryOperator::GREATER:
	case BinaryOperator::GREATER_EQUAL:
		return CheckComparisonOperator(leftType, rightType);
	case BinaryOperator::EQUAL:
	case BinaryOperator::NOT_EQUAL:
		return CheckEqualityOperator(leftType, rightType);
	}

	return MakeError("Unsupported binary operator.");
}

const char* TypeRules::ToString(const Type type)
{
	switch (type)
	{
	case Type::INT: return "int";
	case Type::FLOAT: return "float";
	case Type::BOOL: return "bool";
	case Type::ERROR: return "error";
	}

	return "unknown";
}

TypeCheckResult TypeRules::CheckLogicalOperator(const Type leftType, const Type rightType)
{
	if (leftType != Type::BOOL || rightType != Type::BOOL)
	{
		return MakeError("Logical operator expects bool operands.");
	}

	return TypeCheckResult::Success(Type::BOOL, {});
}

TypeCheckResult TypeRules::CheckArithmeticOperator(const Type leftType, const Type rightType)
{
	if (!IsNumeric(leftType) || !IsNumeric(rightType))
	{
		return MakeError("Arithmetic operator expects numeric operands.");
	}

	return TypeCheckResult::Success(MergeNumeric(leftType, rightType), {});
}

TypeCheckResult TypeRules::CheckModuloOperator(const Type leftType, const Type rightType)
{
	if (leftType != Type::INT || rightType != Type::INT)
	{
		return MakeError("Modulo operator expects int operands.");
	}

	return TypeCheckResult::Success(Type::INT, {});
}

TypeCheckResult TypeRules::CheckComparisonOperator(const Type leftType, const Type rightType)
{
	if (!IsNumeric(leftType) || !IsNumeric(rightType))
	{
		return MakeError("Comparison operator expects numeric operands.");
	}

	return TypeCheckResult::Success(Type::BOOL, {});
}

TypeCheckResult TypeRules::CheckEqualityOperator(const Type leftType, const Type rightType)
{
	if (!AreComparable(leftType, rightType))
	{
		return MakeError("Equality operator expects compatible operands.");
	}

	return TypeCheckResult::Success(Type::BOOL, {});
}

TypeCheckResult TypeRules::MakeError(std::string message)
{
	return TypeCheckResult::Error(std::move(message));
}

bool TypeRules::IsNumeric(const Type type)
{
	return type == Type::INT || type == Type::FLOAT;
}

bool TypeRules::AreComparable(const Type left, const Type right)
{
	if (left == right)
	{
		return true;
	}

	return IsNumeric(left) && IsNumeric(right);
}

Type TypeRules::MergeNumeric(const Type left, const Type right)
{
	if (left == Type::FLOAT || right == Type::FLOAT)
	{
		return Type::FLOAT;
	}

	return Type::INT;
}
