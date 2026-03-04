#pragma once

#include "common.h"
#include "value.h"

enum OpCode : uint8_t
{
	OP_CONSTANT,
	OP_RETURN,
};

class Chunk
{
public:
	Chunk() = default;
	~Chunk() = default;

	void Write(uint8_t byte, int line);
	int AddConstant(Value value);

	const std::vector<uint8_t>& GetCode() const;
	const std::vector<int>& GetLines() const;
	Value GetConstant(int index) const;

private:
	std::vector<uint8_t> m_code;
	std::vector<int> m_lines;
	std::vector<Value> m_constants{};
};
