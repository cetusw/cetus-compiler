#pragma once

#include "LALR.h"
#include "src/frontend/syntax/grammar/SemanticTag.h"
#include "src/frontend/syntax/grammar/Symbol.h"
#include <vector>

struct ParserRule
{
	int index{};
	Symbol lhs;
	std::vector<Symbol> rhs;
	SemanticTag semanticTag = SemanticTag::NONE;
};

struct PreparedGrammar
{
	ParseTable table;
	std::vector<ParserRule> rules;
	Symbol startSymbol;
	Symbol eofSymbol;
};
