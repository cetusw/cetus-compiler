#pragma once
#include "../types/chunk.h"
#include <string>
#include <unordered_map>

class BytecodeParser
{
public:
	BytecodeParser() = default;
	std::shared_ptr<ObjFunction> Parse(const std::string& path);

private:
	void ProcessLine(const std::string& line);
	void HandleDirective(const std::string& line);
	static std::string ProcessStringLiteral(const std::string& input);

	void ParseConstant(std::stringstream& ss);
	void ParseInstruction(std::stringstream& ss, int lineNum) const;

	std::shared_ptr<ObjFunction> GetFunction(const std::string& name);

	std::unordered_map<std::string, std::shared_ptr<ObjFunction>> m_funcRegistry;
	std::shared_ptr<ObjFunction> m_currentFunc = nullptr;
	enum class Section
	{
		NONE,
		CONSTANTS,
		CODE
	} m_section
		= Section::NONE;
};