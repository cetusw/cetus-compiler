#pragma once

#include "Grammar.h"
#include "GrammarLineParser.h"
#include <string>
#include <vector>

class GrammarLoader
{
public:
	static Grammar LoadFromFile(const std::string& path);
	static Grammar LoadFromLines(const std::vector<std::string>& lines);

private:
	static std::string ParseLine(const std::string& line, Grammar& grammar);
	static void AddProductions(Grammar& grammar, const ParsedGrammarLine& line);
};
