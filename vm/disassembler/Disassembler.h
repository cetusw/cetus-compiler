#pragma once

#include "../types/chunk.h"
#include <string>

namespace disassembler
{
void DisassembleChunk(const Chunk& chunk, const std::string& name);
int DisassembleInstruction(const Chunk& chunk, int offset);
} // namespace disassembler
