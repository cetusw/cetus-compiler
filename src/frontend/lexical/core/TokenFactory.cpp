#include "TokenFactory.h"

#include <utility>

Token TokenFactory::Make(const TokenType type, std::string lexeme, const int line)
{
	return { type, std::move(lexeme), line };
}

Token TokenFactory::MakeSynthetic(const TokenType type, std::string lexeme, const int line)
{
	return Make(type, std::move(lexeme), line);
}
