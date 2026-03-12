#include "chunk.h"

void Chunk::Write(const uint8_t byte, const int line)
{
	m_code.push_back(byte);
	m_lines.push_back(line);
}

int Chunk::AddConstant(const Value& value)
{
	m_constants.push_back(value);
	return static_cast<int>(m_constants.size() - 1);
}

const std::vector<uint8_t>& Chunk::GetCode() const
{
	return m_code;
}

const std::vector<int>& Chunk::GetLines() const
{
	return m_lines;
}

Value Chunk::GetConstant(const int index) const
{
	return m_constants[index];
}