#pragma once
#include "src/vm/types/OpCode.h"
#include <string>
#include <unordered_map>

enum class OperandType
{
	NONE,
	BYTE,
	SHORT
};

struct OpCodeInfo
{
	OpCode opcode;
	OperandType operand;
};

static const std::unordered_map<std::string, OpCodeInfo> INSTRUCTIONS = {
	{ "pop", { OP_POP, OperandType::NONE } },
	{ "add", { OP_ADD, OperandType::NONE } },
	{ "sub", { OP_SUBTRACT, OperandType::NONE } },
	{ "mul", { OP_MULTIPLY, OperandType::NONE } },
	{ "div", { OP_DIVIDE, OperandType::NONE } },
	{ "mod", { OP_MODULO, OperandType::NONE } },
	{ "neg", { OP_NEGATE, OperandType::NONE } },
	{ "eq", { OP_EQUAL, OperandType::NONE } },
	{ "ne", { OP_NOT_EQUAL, OperandType::NONE } },
	{ "gt", { OP_GREATER, OperandType::NONE } },
	{ "lt", { OP_LESS, OperandType::NONE } },
	{ "ge", { OP_GREATER_OR_EQUAL, OperandType::NONE } },
	{ "le", { OP_LESS_OR_EQUAL, OperandType::NONE } },
	{ "return", { OP_RETURN, OperandType::NONE } },

	{ "const", { OP_CONSTANT, OperandType::BYTE } },
	{ "get_local", { OP_GET_LOCAL, OperandType::BYTE } },
	{ "set_local", { OP_SET_LOCAL, OperandType::BYTE } },
	{ "call", { OP_CALL, OperandType::BYTE } },

	{ "jump", { OP_JUMP, OperandType::SHORT } },
	{ "jump_if_false", { OP_JUMP_IF_FALSE, OperandType::SHORT } },
	{ "loop", { OP_LOOP, OperandType::SHORT } }
};
