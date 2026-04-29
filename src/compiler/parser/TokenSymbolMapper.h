#pragma once

#include "src/common/grammar/Symbol.h"
#include "src/common/types/Token.h"

class TokenSymbolMapper
{
public:
	static Symbol MapTokenToGrammarSymbol(const Token& token);
};
