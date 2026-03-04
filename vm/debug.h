#pragma once

#include "chunk.h"
#include <string>

namespace debug
{
void DisassembleChunk(const Chunk& chunk, const std::string& name);
int DisassembleInstruction(const Chunk& chunk, int offset);
} // namespace debug
