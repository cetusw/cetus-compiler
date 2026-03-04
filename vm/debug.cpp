#include "debug.h"
#include <cstdio>
#include <iostream>

namespace debug
{
static int SimpleInstruction(const std::string& name, const int offset)
{
	std::cout << name << std::endl;
	return offset + 1;
}

void DisassembleChunk(const Chunk& chunk, const std::string& name)
{
	std::cout << "== " << name << " ==" << std::endl;

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
	case OP_RETURN:
		return SimpleInstruction("OP_RETURN", offset);
	default:
		std::cout << "Unknown opcode " << static_cast<int>(instruction) << std::endl;
		return offset + 1;
	}
}

} // namespace debug
