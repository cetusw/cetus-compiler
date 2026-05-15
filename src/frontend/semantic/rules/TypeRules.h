#pragma once

#include "TypeCheckResult.h"
#include "src/frontend/syntax/ast/ASTNode.h"

#include <string>

class TypeRules
{
public:
	[[nodiscard]] static TypeCheckResult CheckUnaryOperator(UnaryOperator op, Type operandType);
	[[nodiscard]] static TypeCheckResult CheckBinaryOperator(BinaryOperator op, Type leftType, Type rightType);
	[[nodiscard]] static const char* ToString(Type type);

private:
	[[nodiscard]] static TypeCheckResult CheckLogicalOperator(Type leftType, Type rightType);
	[[nodiscard]] static TypeCheckResult CheckArithmeticOperator(Type leftType, Type rightType);
	[[nodiscard]] static TypeCheckResult CheckModuloOperator(Type leftType, Type rightType);
	[[nodiscard]] static TypeCheckResult CheckComparisonOperator(Type leftType, Type rightType);
	[[nodiscard]] static TypeCheckResult CheckEqualityOperator(Type leftType, Type rightType);
	[[nodiscard]] static TypeCheckResult MakeError(std::string message);
	[[nodiscard]] static bool IsNumeric(Type type);
	[[nodiscard]] static bool AreComparable(Type left, Type right);
	[[nodiscard]] static Type MergeNumeric(Type left, Type right);
};
