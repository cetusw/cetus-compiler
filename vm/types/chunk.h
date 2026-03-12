#pragma once

#include "../common.h"
#include "value.h"

class Chunk
{
public:
	Chunk() = default;
	~Chunk() = default;

	void Write(uint8_t byte, int line);
	int AddConstant(Value value);

	[[nodiscard]] const std::vector<uint8_t>& GetCode() const;
	[[nodiscard]] const std::vector<int>& GetLines() const;
	[[nodiscard]] Value GetConstant(int index) const;

private:
	std::vector<uint8_t> m_code;
	std::vector<int> m_lines;
	std::vector<Value> m_constants{};
};
