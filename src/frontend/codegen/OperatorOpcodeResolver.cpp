#include "OperatorOpcodeResolver.h"

#include <unordered_map>

namespace
{
const std::unordered_map<UnaryOperator, OpCode> UNARY_OPCODE_BY_OPERATOR = {
	{ UnaryOperator::NEGATE, OP_NEGATE },
	{ UnaryOperator::NOT, OP_NOT },
};

const std::unordered_map<BinaryOperator, OpCode> BINARY_OPCODE_BY_OPERATOR = {
	{ BinaryOperator::ADD, OP_ADD },
	{ BinaryOperator::SUBTRACT, OP_SUBTRACT },
	{ BinaryOperator::MULTIPLY, OP_MULTIPLY },
	{ BinaryOperator::DIVIDE, OP_DIVIDE },
	{ BinaryOperator::MODULO, OP_MODULO },
	{ BinaryOperator::LESS, OP_LESS },
	{ BinaryOperator::LESS_EQUAL, OP_LESS_OR_EQUAL },
	{ BinaryOperator::NOT_EQUAL, OP_NOT_EQUAL },
	{ BinaryOperator::EQUAL, OP_EQUAL },
	{ BinaryOperator::GREATER, OP_GREATER },
	{ BinaryOperator::GREATER_EQUAL, OP_GREATER_OR_EQUAL },
};
}

std::optional<OpCode> OperatorOpcodeResolver::Resolve(const UnaryOperator op)
{
	const auto opcode = UNARY_OPCODE_BY_OPERATOR.find(op);
	if (opcode == UNARY_OPCODE_BY_OPERATOR.end())
	{
		return std::nullopt;
	}

	return opcode->second;
}

std::optional<OpCode> OperatorOpcodeResolver::Resolve(const BinaryOperator op)
{
	const auto opcode = BINARY_OPCODE_BY_OPERATOR.find(op);
	if (opcode == BINARY_OPCODE_BY_OPERATOR.end())
	{
		return std::nullopt;
	}

	return opcode->second;
}
