#pragma once

#include "../types/chunk.h"
#include "ConstantInstruction.h"
#include "Instruction.h"
#include "ReturnInstruction.h"
#include <array>

class InstructionRegistry
{
public:
	InstructionRegistry()
	{
		m_table[OP_CONSTANT] = std::make_unique<ConstantInstruction>();
		m_table[OP_RETURN] = std::make_unique<ReturnInstruction>();
	}

	const Instruction* Get(const uint8_t opcode) const
	{
		return m_table[opcode].get();
	}

private:
	std::array<std::unique_ptr<Instruction>, TOTAL_POSSIBLE_OPCODES> m_table;
};
