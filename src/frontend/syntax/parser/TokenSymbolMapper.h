#pragma once

#include "src/frontend/lexical/core/Token.h"
#include "src/frontend/syntax/grammar/Symbol.h"

class TokenSymbolMapper
{
public:
	static Symbol MapTokenToGrammarSymbol(const Token& token);
};
