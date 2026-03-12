#include "BytecodeParser.h"
#include "../objects/ObjString.h"
#include <iostream>

bool BytecodeParser::ParseFile(const std::string& path, Chunk& chunk)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cerr << "Could not open file: " << path << std::endl;
		return false;
	}

	std::string line;
	auto currentSection = Section::NONE;

	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
		{
			continue;
		}
		if (line == ".constants")
		{
			currentSection = Section::CONSTANTS;
			continue;
		}
		if (line == ".code")
		{
			currentSection = Section::CODE;
			continue;
		}
		if (line.starts_with(".") || line == ".def" || line == ".end_def")
		{
			continue;
		}
		if (currentSection == Section::CONSTANTS)
		{
			ParseConstant(line, chunk);
		}
		else if (currentSection == Section::CODE)
		{
			ParseInstruction(line, chunk);
		}
	}
	return true;
}

void BytecodeParser::ParseConstant(const std::string& line, Chunk& chunk)
{
	std::stringstream ss(line);
	std::string type, value;
	ss >> type;
	std::getline(ss >> std::ws, value);

	if (type == "number")
	{
		chunk.AddConstant(Value(std::stod(value)));
	}
	else if (type == "string")
	{
		if (value.length() >= 2 && value.front() == '"' && value.back() == '"')
		{
			value = value.substr(1, value.length() - 2);
		}
		chunk.AddConstant(Value(std::make_shared<ObjString>(value)));
	}
	else if (type == "bool")
	{
		chunk.AddConstant(Value(value == "true"));
	}
	else
	{
		std::cerr << "Unknown constant type: " << type << std::endl;
	}
}

void BytecodeParser::ParseInstruction(const std::string& line, Chunk& chunk)
{
	std::stringstream ss(line);
	int lineNum;
	std::string mnemonic;
	ss >> lineNum >> mnemonic;

	const OpCode opcode = StringToOpCode(mnemonic);
	chunk.Write(opcode, lineNum);

	if (static_cast<uint8_t>(opcode) == static_cast<uint8_t>(OP_CONSTANT)
		|| static_cast<uint8_t>(opcode) == static_cast<uint8_t>(OP_GET_LOCAL)
		|| static_cast<uint8_t>(opcode) == static_cast<uint8_t>(OP_SET_LOCAL))
	{
		int operand;
		if (ss >> operand)
		{
			chunk.Write(static_cast<uint8_t>(operand), lineNum);
		}
	}
}

OpCode BytecodeParser::StringToOpCode(const std::string& mnemonic)
{
	static std::unordered_map<std::string, OpCode> mapping = {
		{ "const", OP_CONSTANT },
		{ "pop", OP_POP },
		{ "get_local", OP_GET_LOCAL },
		{ "set_local", OP_SET_LOCAL },
		{ "add", OP_ADD },
		{ "sub", OP_SUBTRACT },
		{ "mul", OP_MULTIPLY },
		{ "div", OP_DIVIDE },
		{ "mod", OP_MODULO },
		{ "neg", OP_NEGATE },
		{ "eq", OP_EQUAL },
		{ "ne", OP_NOT_EQUAL },
		{ "gt", OP_GREATER },
		{ "lt", OP_LESS },
		{ "ge", OP_GREATER_OR_EQUAL },
		{ "le", OP_LESS_OR_EQUAL },
		{ "return", OP_RETURN }
	};

	if (mapping.contains(mnemonic))
	{
		return mapping[mnemonic];
	}

	std::cerr << "Unknown mnemonic: " << mnemonic << ". Defaulting to return." << std::endl;
	return OP_RETURN;
}