#pragma once
#include <cstdint>

enum OpCode : uint8_t
{
	OP_CONSTANT,
	OP_NEGATE,
	OP_RETURN,
};