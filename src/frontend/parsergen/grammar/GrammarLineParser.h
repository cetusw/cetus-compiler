#pragma once

#include "src/frontend/syntax/table/SemanticTag.h"
#include "src/frontend/syntax/table/Symbol.h"
#include <string>
#include <vector>

struct ParsedGrammarLine
{
	Symbol lhs;
	std::vector<std::string> rhsTokens;
	SemanticTag semanticTag = SemanticTag::NONE;
	bool hasSemanticTag = false;
};

class GrammarLineParser
{
public:
	[[nodiscard]] static ParsedGrammarLine Parse(const std::string& line);
	[[nodiscard]] static Symbol ParseSymbolToken(const std::string& token);

private:
	[[nodiscard]] static bool IsNonTerminal(const std::string& value);
};
