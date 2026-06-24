#pragma once

#include "TypeCheckResult.h"
#include "src/frontend/ast/ASTNode.h"

#include <string>

class TypeRules
{
public:
	[[nodiscard]] static TypeCheckResult CheckUnaryOperator(UnaryOperator op, const TypeDescriptor& operandType);
	[[nodiscard]] static TypeCheckResult CheckBinaryOperator(BinaryOperator op, const TypeDescriptor& leftType, const TypeDescriptor& rightType);
	[[nodiscard]] static std::string ToString(const TypeDescriptor& type);
	[[nodiscard]] static bool IsAssignable(const TypeDescriptor& expected, const TypeDescriptor& actual);

private:
	[[nodiscard]] static TypeCheckResult CheckLogicalOperator(const TypeDescriptor& leftType, const TypeDescriptor& rightType);
	[[nodiscard]] static TypeCheckResult CheckArithmeticOperator(const TypeDescriptor& leftType, const TypeDescriptor& rightType);
	[[nodiscard]] static TypeCheckResult CheckModuloOperator(const TypeDescriptor& leftType, const TypeDescriptor& rightType);
	[[nodiscard]] static TypeCheckResult CheckComparisonOperator(const TypeDescriptor& leftType, const TypeDescriptor& rightType);
	[[nodiscard]] static TypeCheckResult CheckEqualityOperator(const TypeDescriptor& leftType, const TypeDescriptor& rightType);
	[[nodiscard]] static TypeCheckResult MakeError(std::string message);
	[[nodiscard]] static bool IsNumeric(const TypeDescriptor& type);
	[[nodiscard]] static bool IsFalsey(const TypeDescriptor& type);
	[[nodiscard]] static bool AreComparable(const TypeDescriptor& left, const TypeDescriptor& right);
	[[nodiscard]] static TypeDescriptor MergeNumeric(const TypeDescriptor& left, const TypeDescriptor& right);
};
