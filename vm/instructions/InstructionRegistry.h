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
		m_table[OP_ADD] = std::make_unique<BinaryInstruction<AddOp>>();
		m_table[OP_SUBTRACT] = std::make_unique<BinaryInstruction<SubOp>>();
		m_table[OP_MULTIPLY] = std::make_unique<BinaryInstruction<MulOp>>();
		m_table[OP_DIVIDE] = std::make_unique<BinaryInstruction<DivOp>>();
		m_table[OP_MODULO] = std::make_unique<BinaryInstruction<ModOp>>();

		m_table[OP_GREATER] = std::make_unique<BinaryInstruction<GreaterOp>>();
		m_table[OP_LESS] = std::make_unique<BinaryInstruction<LessOp>>();
		m_table[OP_GREATER_OR_EQUAL] = std::make_unique<BinaryInstruction<GreaterOrEqualOp>>();
		m_table[OP_LESS_OR_EQUAL] = std::make_unique<BinaryInstruction<LessOrEqualOp>>();
		m_table[OP_EQUAL] = std::make_unique<BinaryInstruction<EqualOp>>();
		m_table[OP_NOT_EQUAL] = std::make_unique<BinaryInstruction<NotEqualOp>>();

		m_table[OP_RETURN] = std::make_unique<ReturnInstruction>();
	}

	[[nodiscard]] const Instruction* Get(const uint8_t opcode) const
	{
		return m_table[opcode].get();
	}

private:
	std::array<std::unique_ptr<Instruction>, TOTAL_POSSIBLE_OPCODES> m_table;
};
