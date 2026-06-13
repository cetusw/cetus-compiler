#pragma once
#include "../types/OpCode.h"
#include "AssertInstruction.h"
#include "ArrayInstruction.h"
#include "BinaryInstruction.h"
#include "CallInstruction.h"
#include "ConstantInstruction.h"
#include "DefineGlobalInstruction.h"
#include "GetGlobalInstruction.h"
#include "GetIndexInstruction.h"
#include "GetLocalInstruction.h"
#include "GetMemberInstruction.h"
#include "GetSliceInstruction.h"
#include "Instruction.h"
#include "JumpIfFalseInstruction.h"
#include "JumpInstruction.h"
#include "LoopInstruction.h"
#include "NegateInstruction.h"
#include "NotInstruction.h"
#include "OverInstruction.h"
#include "PopInstruction.h"
#include "PointerInstruction.h"
#include "RefGlobalInstruction.h"
#include "RefIndexInstruction.h"
#include "RefLocalInstruction.h"
#include "RefMemberInstruction.h"
#include "ReturnInstruction.h"
#include "SetGlobalInstruction.h"
#include "SetIndexInstruction.h"
#include "SetLocalInstruction.h"
#include "SetMemberInstruction.h"
#include "SliceInstruction.h"
#include "StructInstruction.h"
#include <array>

class InstructionRegistry
{
public:
	InstructionRegistry()
	{
		m_table[OP_CONSTANT] = std::make_unique<ConstantInstruction>();
		m_table[OP_GET_LOCAL] = std::make_unique<GetLocalInstruction>();
		m_table[OP_SET_LOCAL] = std::make_unique<SetLocalInstruction>();
		m_table[OP_REF_LOCAL] = std::make_unique<RefLocalInstruction>();
		m_table[OP_DEFINE_GLOBAL] = std::make_unique<DefineGlobalInstruction>();
		m_table[OP_GET_GLOBAL] = std::make_unique<GetGlobalInstruction>();
		m_table[OP_SET_GLOBAL] = std::make_unique<SetGlobalInstruction>();
		m_table[OP_REF_GLOBAL] = std::make_unique<RefGlobalInstruction>();
		m_table[OP_POP] = std::make_unique<PopInstruction>();

		m_table[OP_NEGATE] = std::make_unique<NegateInstruction>();
		m_table[OP_NOT] = std::make_unique<NotInstruction>();
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

		m_table[OP_JUMP] = std::make_unique<JumpInstruction>();
		m_table[OP_JUMP_IF_FALSE] = std::make_unique<JumpIfFalseInstruction>();
		m_table[OP_LOOP] = std::make_unique<LoopInstruction>();

		m_table[OP_CALL] = std::make_unique<CallInstruction>();
		m_table[OP_ARRAY] = std::make_unique<ArrayInstruction>();
		m_table[OP_SLICE] = std::make_unique<SliceInstruction>();
		m_table[OP_GET_SLICE] = std::make_unique<GetSliceInstruction>();
		m_table[OP_GET_INDEX] = std::make_unique<GetIndexInstruction>();
		m_table[OP_SET_INDEX] = std::make_unique<SetIndexInstruction>();
		m_table[OP_STRUCT] = std::make_unique<StructInstruction>();
		m_table[OP_POINTER] = std::make_unique<PointerInstruction>();
		m_table[OP_OVER] = std::make_unique<OverInstruction>();
		m_table[OP_REF_INDEX] = std::make_unique<RefIndexInstruction>();
		m_table[OP_REF_MEMBER] = std::make_unique<RefMemberInstruction>();
		m_table[OP_GET_MEMBER] = std::make_unique<GetMemberInstruction>();
		m_table[OP_SET_MEMBER] = std::make_unique<SetMemberInstruction>();
		m_table[OP_ASSERT] = std::make_unique<AssertInstruction>();

		m_table[OP_RETURN] = std::make_unique<ReturnInstruction>();
	}

	[[nodiscard]] const Instruction* Get(const uint8_t opcode) const
	{
		return m_table[opcode].get();
	}

private:
	std::array<std::unique_ptr<Instruction>, TOTAL_POSSIBLE_OPCODES> m_table;
};
