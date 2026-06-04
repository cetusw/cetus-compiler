#pragma once

#include "src/backend/vm/types/OpCode.h"
#include "src/backend/vm/types/Value.h"
#include <cstdint>
#include <optional>
#include <string>

class Chunk;

class BytecodeEmitter
{
public:
	BytecodeEmitter(Chunk& chunk, std::optional<std::string>& error);

	void EmitOpcode(OpCode opcode) const;
	void EmitOperandByte(int value) const;
	void EmitConstant(const Value& value) const;
	[[nodiscard]] int EmitJump(OpCode opcode) const;
	void PatchJump(int jumpOffset) const;
	void EmitLocalLoad(int slot) const;
	void EmitGlobalLoad(const std::string& name) const;
	void EmitGlobalDefine(const std::string& name) const;
	void EmitGlobalSet(const std::string& name) const;

private:
	static constexpr int DEFAULT_LINE = 1;

	void Fail(std::string message) const;
	void EmitByte(uint8_t byte) const;
	void EmitShortOperand(int value, int patchOffset) const;

	Chunk& m_chunk;
	std::optional<std::string>& m_error;
};
