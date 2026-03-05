#pragma once

#include "../types/chunk.h"
#include "BinaryInstruction.h"
#include "ConstantInstruction.h"
#include "Instruction.h"
#include "NegateInstruction.h"
#include "ReturnInstruction.h"
#include "types/OpCode.h"
#include <array>

class InstructionRegistry
{
public:
	InstructionRegistry()
	{
		m_table[OP_CONSTANT] = std::make_unique<ConstantInstruction>();
		m_table[OP_NEGATE] = std::make_unique<NegateInstruction>();
		m_table[OP_RETURN] = std::make_unique<ReturnInstruction>();
		m_table[OP_ADD] = std::make_unique<BinaryInstruction<AddOp>>();
		m_table[OP_SUBTRACT] = std::make_unique<BinaryInstruction<SubOp>>();
		m_table[OP_MULTIPLY] = std::make_unique<BinaryInstruction<MulOp>>();
		m_table[OP_DIVIDE] = std::make_unique<BinaryInstruction<DivOp>>();
	}

	const Instruction* Get(const uint8_t opcode) const
	{
		return m_table[opcode].get();
	}

private:
	std::array<std::unique_ptr<Instruction>, TOTAL_POSSIBLE_OPCODES> m_table;
};
