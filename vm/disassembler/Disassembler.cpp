#include "Disassembler.h"
#include "types/OpCode.h"
#include <cstdio>

namespace disassembler
{
static int SimpleInstruction(const std::string& name, const int offset)
{
	std::printf("%-16s\n", name.c_str());
	return offset + 1;
}

static int ConstantInstruction(const std::string& name, const Chunk& chunk, const int offset)
{
	const auto& code = chunk.GetCode();
	const uint8_t constantIndex = code[offset + 1];
	std::printf("%-16s %4d '", name.c_str(), constantIndex);

	const Value value = chunk.GetConstant(constantIndex);
	value.Print();
	std::printf("\n");

	return offset + 2;
}

void DisassembleChunk(const Chunk& chunk, const std::string& name)
{
	std::printf("%s%s%s\n", "== ", name.c_str(), " ==");

	const auto& code = chunk.GetCode();

	for (int offset = 0; offset < static_cast<int>(code.size());)
	{
		offset = DisassembleInstruction(chunk, offset);
	}
}

int DisassembleInstruction(const Chunk& chunk, const int offset)
{
	std::printf("%04d ", offset);

	const auto& code = chunk.GetCode();
	uint8_t instruction = code[offset];

	switch (static_cast<OpCode>(instruction))
	{
	case OP_CONSTANT:
		return ConstantInstruction("OP_CONSTANT", chunk, offset);
	case OP_NEGATE:
		return SimpleInstruction("OP_NEGATE", offset);
	case OP_ADD:
		return SimpleInstruction("OP_ADD", offset);
	case OP_SUBTRACT:
		return SimpleInstruction("OP_SUBTRACT", offset);
	case OP_MULTIPLY:
		return SimpleInstruction("OP_MULTIPLY", offset);
	case OP_DIVIDE:
		return SimpleInstruction("OP_DIVIDE", offset);
	case OP_RETURN:
		return SimpleInstruction("OP_RETURN", offset);
	case OP_EQUAL:
		return SimpleInstruction("OP_EQUAL", offset);
	case OP_GREATER:
		return SimpleInstruction("OP_GREATER", offset);
	case OP_LESS:
		return SimpleInstruction("OP_LESS", offset);
	case OP_GREATER_OR_EQUAL:
		return SimpleInstruction("OP_GREATER_OR_EQUAL", offset);
	case OP_LESS_OR_EQUAL:
		return SimpleInstruction("OP_LESS_OR_EQUAL", offset);

	default:
		std::printf("%s %i\n", "Unknown opcode:", static_cast<int>(instruction));
		return offset + 1;
	}
}

} // namespace disassembler
