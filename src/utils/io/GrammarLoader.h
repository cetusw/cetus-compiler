#pragma once

#include "../../common/grammar/Grammar.h"
#include <string>

class GrammarLoader
{
public:
	static Grammar LoadFromFile(const std::string& path);

private:
	static bool IsTerminal(const std::string& value);
	static std::string ParseLine(const std::string& line, Grammar& grammar);
	static void AddProductions(Grammar& grammar, const Symbol& lhs, const std::vector<std::string>& tokens);
};