#pragma once

#include "src/frontend/lexical/Token.h"
#include "src/frontend/syntax/grammar/Symbol.h"

class TokenSymbolMapper
{
public:
	static Symbol MapTokenToGrammarSymbol(const Token& token);
};
