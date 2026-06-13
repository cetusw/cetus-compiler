#include "BytecodeEmitter.h"

#include "src/backend/vm/objects/ObjAssertionMetadata.h"
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

int BytecodeEmitter::CurrentOffset() const
{
	return m_chunk.GetCodeSize();
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

void BytecodeEmitter::EmitLoop(const int loopStart) const
{
	const int loopOpcodeOffset = m_chunk.GetCodeSize();
	EmitOpcode(OP_LOOP);
	const int offset = loopOpcodeOffset - loopStart;
	if (offset < 0 || offset > std::numeric_limits<uint16_t>::max())
	{
		Fail("Loop offset exceeds 2-byte limit.");
		return;
	}

	EmitByte(static_cast<uint8_t>(offset >> 8 & 0xff));
	EmitByte(static_cast<uint8_t>(offset & 0xff));
}

void BytecodeEmitter::EmitLocalLoad(const int slot) const
{
	EmitOpcode(OP_GET_LOCAL);
	EmitOperandByte(slot);
}

void BytecodeEmitter::EmitLocalSet(const int slot) const
{
	EmitOpcode(OP_SET_LOCAL);
	EmitOperandByte(slot);
}

void BytecodeEmitter::EmitLocalRef(const int slot) const
{
	EmitOpcode(OP_REF_LOCAL);
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

void BytecodeEmitter::EmitGlobalRef(const std::string& name) const
{
	EmitOpcode(OP_REF_GLOBAL);
	const int constantIndex = m_chunk.AddConstant(Value(std::make_shared<ObjString>(name)));
	EmitOperandByte(constantIndex);
}

void BytecodeEmitter::EmitArray(const int length) const
{
	EmitOpcode(OP_ARRAY);
	EmitOperandByte(length);
}

void BytecodeEmitter::EmitSlice(const int length) const
{
	EmitOpcode(OP_SLICE);
	EmitOperandByte(length);
}

void BytecodeEmitter::EmitSliceLoad(const bool hasStart, const bool hasEnd) const
{
	EmitOpcode(OP_GET_SLICE);
	int flags = 0;
	if (hasStart)
	{
		flags |= 0x01;
	}
	if (hasEnd)
	{
		flags |= 0x02;
	}
	EmitOperandByte(flags);
}

void BytecodeEmitter::EmitIndexLoad() const
{
	EmitOpcode(OP_GET_INDEX);
}

void BytecodeEmitter::EmitIndexSet() const
{
	EmitOpcode(OP_SET_INDEX);
}

void BytecodeEmitter::EmitStruct(const std::string& typeName, const std::vector<std::string>& fieldNames) const
{
	EmitOpcode(OP_STRUCT);
	const int typeNameIndex = m_chunk.AddConstant(Value(std::make_shared<ObjString>(typeName)));
	EmitOperandByte(typeNameIndex);
	EmitOperandByte(static_cast<int>(fieldNames.size()));
	for (const std::string& fieldName : fieldNames)
	{
		const int fieldNameIndex = m_chunk.AddConstant(Value(std::make_shared<ObjString>(fieldName)));
		EmitOperandByte(fieldNameIndex);
	}
}

void BytecodeEmitter::EmitPointer() const
{
	EmitOpcode(OP_POINTER);
}

void BytecodeEmitter::EmitOver() const
{
	EmitOpcode(OP_OVER);
}

void BytecodeEmitter::EmitMemberRef(const std::string& fieldName) const
{
	EmitOpcode(OP_REF_MEMBER);
	const int constantIndex = m_chunk.AddConstant(Value(std::make_shared<ObjString>(fieldName)));
	EmitOperandByte(constantIndex);
}

void BytecodeEmitter::EmitIndexRef() const
{
	EmitOpcode(OP_REF_INDEX);
}

void BytecodeEmitter::EmitMemberLoad(const std::string& fieldName) const
{
	EmitOpcode(OP_GET_MEMBER);
	const int constantIndex = m_chunk.AddConstant(Value(std::make_shared<ObjString>(fieldName)));
	EmitOperandByte(constantIndex);
}

void BytecodeEmitter::EmitMemberSet(const std::string& fieldName) const
{
	EmitOpcode(OP_SET_MEMBER);
	const int constantIndex = m_chunk.AddConstant(Value(std::make_shared<ObjString>(fieldName)));
	EmitOperandByte(constantIndex);
}

void BytecodeEmitter::EmitAssert(const int sourceLine, const std::string& sourceText) const
{
	EmitOpcode(OP_ASSERT);
	const AssertionDescriptor descriptor{ sourceLine, sourceText, std::nullopt };
	const int metadataIndex = m_chunk.AddConstant(Value(std::make_shared<ObjAssertionMetadata>(descriptor)));
	EmitOperandByte(metadataIndex);
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
