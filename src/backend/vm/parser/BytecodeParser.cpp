#include "BytecodeParser.h"
#include "../objects/ObjString.h"
#include "Types.h"
#include "src/backend/vm/objects/ObjFunction.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

std::shared_ptr<ObjFunction> BytecodeParser::Parse(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		return nullptr;
	}

	std::string line;
	while (std::getline(file, line))
	{
		ProcessLine(line);
	}

	return m_funcRegistry.contains("main")
		? m_funcRegistry["main"]
		: m_currentFunc;
}

void BytecodeParser::ProcessLine(const std::string& line)
{
	if (line.empty() || line[0] == '#')
	{
		return;
	}

	if (line[0] == '.')
	{
		HandleDirective(line);
		return;
	}

	std::stringstream ss(line);
	if (m_section == Section::CONSTANTS)
	{
		ParseConstant(ss);
	}
	else if (m_section == Section::CODE)
	{
		int lineNum;
		ss >> lineNum;
		ParseInstruction(ss, lineNum);
	}
}

void BytecodeParser::HandleDirective(const std::string& line)
{
	std::stringstream ss(line);
	std::string directive;
	ss >> directive;

	if (directive == ".def")
	{
		std::string name;
		ss >> name;
		m_currentFunc = GetFunction(name);
	}
	else if (directive == ".arity")
	{
		if (m_currentFunc)
		{
			ss >> m_currentFunc->arity;
		}
	}
	else if (directive == ".constants")
	{
		m_section = Section::CONSTANTS;
	}
	else if (directive == ".code")
	{
		m_section = Section::CODE;
	}
}

std::string BytecodeParser::ProcessStringLiteral(const std::string& input)
{
	if (input.length() < 2 || input.front() != '"' || input.back() != '"')
	{
		return input;
	}

	std::string result;
	result.reserve(input.length() - 2);

	for (size_t i = 1; i < input.length() - 1; ++i)
	{
		if (input[i] == '\\' && i + 1 < input.length() - 1)
		{
			switch (input[++i])
			{
			case 'n':
				result += '\n';
				break;
			case 't':
				result += '\t';
				break;
			case 'r':
				result += '\r';
				break;
			case '\\':
				result += '\\';
				break;
			case '"':
				result += '"';
				break;
			case '\'':
				result += '\'';
				break;
			default:
				result += input[i];
				break;
			}
		}
		else
		{
			result += input[i];
		}
	}

	return result;
}

void BytecodeParser::ParseConstant(std::stringstream& ss)
{
	if (!m_currentFunc)
	{
		return;
	}

	std::string type, value;
	ss >> type;
	std::getline(ss >> std::ws, value);

	if (type == "number")
	{
		m_currentFunc->chunk.AddConstant(Value(std::stod(value)));
	}
	else if (type == "string")
	{
		std::string processed = ProcessStringLiteral(value);
		m_currentFunc->chunk.AddConstant(Value(std::make_shared<ObjString>(processed)));
	}
	else if (type == "function")
	{
		m_currentFunc->chunk.AddConstant(Value(GetFunction(value)));
	}
}

void BytecodeParser::ParseInstruction(std::stringstream& ss, const int lineNum) const
{
	if (!m_currentFunc)
	{
		return;
	}

	std::string mnemonic;
	ss >> mnemonic;

	if (!INSTRUCTIONS.contains(mnemonic))
	{
		return;
	}

	const auto& info = INSTRUCTIONS.at(mnemonic);
	m_currentFunc->chunk.Write(info.opcode, lineNum);

	if (info.operand == OperandType::BYTE)
	{
		int val;
		ss >> val;
		m_currentFunc->chunk.Write(static_cast<uint8_t>(val), lineNum);
	}
	else if (info.operand == OperandType::SHORT)
	{
		int val;
		ss >> val;
		m_currentFunc->chunk.Write(static_cast<uint8_t>((val >> 8) & 0xff), lineNum);
		m_currentFunc->chunk.Write(static_cast<uint8_t>(val & 0xff), lineNum);
	}
}

std::shared_ptr<ObjFunction> BytecodeParser::GetFunction(const std::string& name)
{
	if (!m_funcRegistry.contains(name))
	{
		const auto func = std::make_shared<ObjFunction>();
		func->name = std::make_shared<ObjString>(name);
		m_funcRegistry[name] = func;
	}
	return m_funcRegistry[name];
}
