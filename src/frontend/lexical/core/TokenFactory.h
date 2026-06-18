#pragma once

#include "src/frontend/lexical/core/Token.h"

#include <string>

class TokenFactory
{
public:
	[[nodiscard]] static Token Make(TokenType type, std::string lexeme, int line);
	[[nodiscard]] static Token MakeSynthetic(TokenType type, std::string lexeme, int line);
};
