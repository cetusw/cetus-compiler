#include "BytecodeEmitter.h"

#include "src/backend/vm/objects/ObjString.h"
#include "src/backend/vm/types/Chunk.h"
#include <limits>

BytecodeEmitter::BytecodeEmitter(Chunk& chunk, std::optional<std::string>& error)
	: m_chunk(chunk)
	, m_error(error)
{
}

void BytecodeEmitter::EmitOpcode(const OpCode opcode) const
{
	EmitByte(opcode);
}

void BytecodeEmitter::EmitOperandByte(const int value) const
{
	if (value < 0 || value > std::numeric_limits<uint8_t>::max())
	{
		Fail("Bytecode operand exceeds 1-byte limit.");
		return;
	}

	EmitByte(static_cast<uint8_t>(value));
}

void BytecodeEmitter::EmitConstant(const Value& value) const
{
	const int constantIndex = m_chunk.AddConstant(value);
	EmitOpcode(OP_CONSTANT);
	EmitOperandByte(constantIndex);
}

int BytecodeEmitter::EmitJump(const OpCode opcode) const
{
	EmitOpcode(opcode);
	EmitByte(0xff);
	EmitByte(0xff);
	return m_chunk.GetCodeSize() - 2;
}

void BytecodeEmitter::PatchJump(const int jumpOffset) const
{
	const int jumpDistance = m_chunk.GetCodeSize() - jumpOffset - 2;
	if (jumpDistance < 0 || jumpDistance > std::numeric_limits<uint16_t>::max())
	{
		Fail("Jump offset exceeds 2-byte limit.");
		return;
	}

	EmitShortOperand(jumpDistance, jumpOffset);
}

void BytecodeEmitter::EmitLocalLoad(const int slot) const
{
	EmitOpcode(OP_GET_LOCAL);
	EmitOperandByte(slot);
}

void BytecodeEmitter::EmitGlobalLoad(const std::string& name) const
{
	EmitOpcode(OP_GET_GLOBAL);
	const int constantIndex = m_chunk.AddConstant(Value(std::make_shared<ObjString>(name)));
	EmitOperandByte(constantIndex);
}

void BytecodeEmitter::EmitGlobalDefine(const std::string& name) const
{
	EmitOpcode(OP_DEFINE_GLOBAL);
	const int constantIndex = m_chunk.AddConstant(Value(std::make_shared<ObjString>(name)));
	EmitOperandByte(constantIndex);
}

void BytecodeEmitter::EmitGlobalSet(const std::string& name) const
{
	EmitOpcode(OP_SET_GLOBAL);
	const int constantIndex = m_chunk.AddConstant(Value(std::make_shared<ObjString>(name)));
	EmitOperandByte(constantIndex);
}

void BytecodeEmitter::Fail(std::string message) const
{
	if (!m_error.has_value())
	{
		m_error = std::move(message);
	}
}

void BytecodeEmitter::EmitByte(const uint8_t byte) const
{
	m_chunk.Write(byte, DEFAULT_LINE);
}

void BytecodeEmitter::EmitShortOperand(const int value, const int patchOffset) const
{
	m_chunk.PatchByte(patchOffset, static_cast<uint8_t>(value >> 8 & 0xff));
	m_chunk.PatchByte(patchOffset + 1, static_cast<uint8_t>(value & 0xff));
}
