#pragma once

#include "LALR.h"
#include "src/common/grammar/Symbol.h"
#include <vector>

struct ParserRule
{
	int index{};
	Symbol lhs;
	std::vector<Symbol> rhs;
};

struct ParserDefinition
{
	ParseTable table;
	std::vector<ParserRule> rules;
	Symbol startSymbol;
	Symbol eofSymbol;
};
