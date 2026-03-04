#include "chunk.h"

void Chunk::Write(const uint8_t byte, const int line)
{
	code.push_back(byte);
	lines.push_back(line);
}

int Chunk::AddConstant(const Value value)
{
	constants.push_back(value);
	return static_cast<int>(constants.size() - 1);
}

const std::vector<uint8_t>& Chunk::GetCode() const
{
	return code;
}

const std::vector<int>& Chunk::GetLines() const
{
	return lines;
}

const std::vector<Value>& Chunk::GetConstants() const
{
	return constants;
}