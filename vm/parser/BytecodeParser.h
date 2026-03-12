#pragma once
#include "../types/OpCode.h"
#include "../types/chunk.h"
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class BytecodeParser
{
public:
	BytecodeParser() = default;
	static bool ParseFile(const std::string& path, Chunk& chunk);

private:
	enum class Section
	{
		NONE,
		CONSTANTS,
		CODE
	};

	static void ParseConstant(const std::string& line, Chunk& chunk);
	static void ParseInstruction(const std::string& line, Chunk& chunk);
	static OpCode StringToOpCode(const std::string& mnemonic);
};