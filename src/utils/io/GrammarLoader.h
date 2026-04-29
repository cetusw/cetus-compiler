#pragma once

#include "../../common/grammar/Grammar.h"
#include <string>

class GrammarLoader
{
public:
	static Grammar LoadFromFile(const std::string& path);

private:
	static bool IsNonTerminal(const std::string& value);
	static Symbol ParseSymbolToken(const std::string& token);
	static Grammar ParseFile(std::ifstream file);
	static std::string ParseLine(const std::string& line, Grammar& grammar);
	static void AddProductions(Grammar& grammar, const Symbol& lhs, const std::vector<std::string>& tokens);
};
