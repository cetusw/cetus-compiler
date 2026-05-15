#pragma once

#include "Grammar.h"
#include <string>
#include <vector>

class GrammarLoader
{
public:
	static Grammar LoadFromFile(const std::string& path);

private:
	struct ParsedLine
	{
		Symbol lhs;
		std::vector<std::string> rhs;
		SemanticTag semanticTag = SemanticTag::NONE;
		bool hasSemanticTag = false;
	};

	static bool IsNonTerminal(const std::string& value);
	static Symbol ParseSymbolToken(const std::string& token);
	static Grammar ParseLines(const std::vector<std::string>& lines);
	static std::string ParseLine(const std::string& line, Grammar& grammar);
	static ParsedLine ParseProductionLine(const std::string& line);
	static void AddProductions(Grammar& grammar, const ParsedLine& line);
};
