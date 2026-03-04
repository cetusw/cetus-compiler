#pragma once

#include "common.h"

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
	const std::vector<Value>& GetConstants() const;

private:
	std::vector<uint8_t> code;
	std::vector<int> lines;
	std::vector<Value> constants{};
};
