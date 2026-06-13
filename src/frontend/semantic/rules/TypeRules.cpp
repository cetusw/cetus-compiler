#include "TypeRules.h"

TypeCheckResult TypeRules::CheckUnaryOperator(const UnaryOperator op, const TypeDescriptor& operandType)
{
	switch (op)
	{
	case UnaryOperator::NEGATE:
		if (!IsNumeric(operandType))
		{
			return MakeError(
				std::string("Unary '-' expects numeric operand, got ") + ToString(operandType));
		}
		return TypeCheckResult::Success(operandType);
	case UnaryOperator::NOT:
		if (!IsFalsey(operandType))
		{
			return MakeError(
				std::string("Unary '!' expects truthy-compatible operand, got ") + ToString(operandType));
		}
		return TypeCheckResult::Success(Type::BOOL);
	}

	return MakeError("Unsupported unary operator.");
}

TypeCheckResult TypeRules::CheckBinaryOperator(
	const BinaryOperator op,
	const TypeDescriptor& leftType,
	const TypeDescriptor& rightType)
{
	switch (op)
	{
	case BinaryOperator::OR:
	case BinaryOperator::AND:
		return CheckLogicalOperator(leftType, rightType);
	case BinaryOperator::ADD:
		// TODO to refactor
		if (leftType == Type::STRING && rightType == Type::STRING)
		{
			return TypeCheckResult::Success(Type::STRING);
		}
		return CheckArithmeticOperator(leftType, rightType);
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

std::string TypeRules::ToString(const TypeDescriptor& type)
{
	return type.ToString();
}

TypeCheckResult TypeRules::CheckLogicalOperator(const TypeDescriptor& leftType, const TypeDescriptor& rightType)
{
	if (!IsFalsey(leftType) || !IsFalsey(rightType))
	{
		return MakeError("Logical operator expects truthy-compatible operands.");
	}

	return TypeCheckResult::Success(Type::BOOL);
}

TypeCheckResult TypeRules::CheckArithmeticOperator(const TypeDescriptor& leftType, const TypeDescriptor& rightType)
{
	if (!IsNumeric(leftType) || !IsNumeric(rightType))
	{
		return MakeError("Arithmetic operator expects numeric operands.");
	}

	return TypeCheckResult::Success(MergeNumeric(leftType, rightType));
}

TypeCheckResult TypeRules::CheckModuloOperator(const TypeDescriptor& leftType, const TypeDescriptor& rightType)
{
	if (leftType != Type::INT || rightType != Type::INT)
	{
		return MakeError("Modulo operator expects int operands.");
	}

	return TypeCheckResult::Success(Type::INT);
}

TypeCheckResult TypeRules::CheckComparisonOperator(const TypeDescriptor& leftType, const TypeDescriptor& rightType)
{
	if (!IsFalsey(leftType) || !IsFalsey(rightType))
	{
		return MakeError("Comparison operator expects truthy-compatible operands.");
	}

	return TypeCheckResult::Success(Type::BOOL);
}

TypeCheckResult TypeRules::CheckEqualityOperator(const TypeDescriptor& leftType, const TypeDescriptor& rightType)
{
	if (!AreComparable(leftType, rightType))
	{
		return MakeError("Equality operator expects compatible operands.");
	}

	return TypeCheckResult::Success(Type::BOOL);
}

TypeCheckResult TypeRules::MakeError(std::string message)
{
	return TypeCheckResult::Error(std::move(message));
}

bool TypeRules::IsNumeric(const TypeDescriptor& type)
{
	return type == Type::INT || type == Type::FLOAT;
}

bool TypeRules::IsFalsey(const TypeDescriptor& type)
{
	return type == Type::BOOL || IsNumeric(type);
}

bool TypeRules::IsAssignable(const TypeDescriptor& expected, const TypeDescriptor& actual)
{
	if (expected == actual)
	{
		return true;
	}

	return actual == Type::NIL && expected.IsNullable();
}

bool TypeRules::AreComparable(const TypeDescriptor& left, const TypeDescriptor& right)
{
	if (IsAssignable(left, right) || IsAssignable(right, left))
	{
		return true;
	}

	return IsFalsey(left) && IsFalsey(right);
}

TypeDescriptor TypeRules::MergeNumeric(const TypeDescriptor& left, const TypeDescriptor& right)
{
	if (left == Type::FLOAT || right == Type::FLOAT)
	{
		return Type::FLOAT;
	}

	return Type::INT;
}
