#include "TokenSymbolMapper.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>

namespace
{
struct TokenTerminalEntry
{
	TokenType type;
	std::string_view terminal;
};

constexpr TokenTerminalEntry TOKEN_TO_TERMINAL[] = {
	{ TokenType::OR_OR, "OR_OR" },
	{ TokenType::AND_AND, "AND_AND" },
	{ TokenType::BIT_AND, "BIT_AND" },
	{ TokenType::SEMICOLON, "SEMICOLON" },
	{ TokenType::IF, "IF" },
	{ TokenType::ELSE, "ELSE" },
	{ TokenType::FOR, "FOR" },
	{ TokenType::BREAK, "BREAK" },
	{ TokenType::CONTINUE, "CONTINUE" },
	{ TokenType::RETURN, "RETURN" },
	{ TokenType::TEST, "TEST" },
	{ TokenType::ASSERT, "ASSERT" },
	{ TokenType::FORALL, "FORALL" },
	{ TokenType::FUNC, "FUNC" },
	{ TokenType::TYPE, "TYPE" },
	{ TokenType::VAR, "VAR" },
	{ TokenType::STRUCT, "STRUCT" },
	{ TokenType::COMMA, "COMMA" },
	{ TokenType::COLON, "COLON" },
	{ TokenType::EQUAL, "EQUAL" },
	{ TokenType::COLON_EQUAL, "COLON_EQUAL" },
	{ TokenType::BANG, "BANG" },
	{ TokenType::TRUE, "TRUE" },
	{ TokenType::FALSE, "FALSE" },
	{ TokenType::NIL, "NIL" },
	{ TokenType::PLUS, "PLUS" },
	{ TokenType::PLUS_PLUS, "PLUS_PLUS" },
	{ TokenType::MINUS, "MINUS" },
	{ TokenType::MINUS_MINUS, "MINUS_MINUS" },
	{ TokenType::STAR, "STAR" },
	{ TokenType::SLASH, "SLASH" },
	{ TokenType::PERCENT, "PERCENT" },
	{ TokenType::LPAREN, "LPAREN" },
	{ TokenType::RPAREN, "RPAREN" },
	{ TokenType::LBRACE, "LBRACE" },
	{ TokenType::RBRACE, "RBRACE" },
	{ TokenType::LBRACKET, "LBRACKET" },
	{ TokenType::RBRACKET, "RBRACKET" },
	{ TokenType::DOT, "DOT" },
	{ TokenType::LESS, "LESS" },
	{ TokenType::LESS_EQUAL, "LESS_EQUAL" },
	{ TokenType::BANG_EQUAL, "BANG_EQUAL" },
	{ TokenType::EQUAL_EQUAL, "EQUAL_EQUAL" },
	{ TokenType::GREATER, "GREATER" },
	{ TokenType::GREATER_EQUAL, "GREATER_EQUAL" },
	{ TokenType::IDENTIFIER, "IDENTIFIER" },
	{ TokenType::TYPE_IDENTIFIER, "TYPE_IDENTIFIER" },
	{ TokenType::INT_LIT, "INT_LIT" },
	{ TokenType::FLOAT_LIT, "FLOAT_LIT" },
	{ TokenType::STRING, "STRING" },
	{ TokenType::EOF_TOKEN, "⊥" },
};

std::string_view TokenTypeToGrammarTerminal(const TokenType type)
{
	const auto entry = std::ranges::find_if(TOKEN_TO_TERMINAL,
		[type](const TokenTerminalEntry& candidate) { return candidate.type == type; });
	if (entry == std::end(TOKEN_TO_TERMINAL))
	{
		return "";
	}
	return entry->terminal;
}
} // namespace

Symbol TokenSymbolMapper::MapTokenToGrammarSymbol(const Token& token)
{
	const std::string_view terminal = TokenTypeToGrammarTerminal(token.type);
	if (terminal.empty())
	{
		throw std::runtime_error(
			"Token is not supported by the current grammar. Token type does not belong to the parser terminal set: '"
			+ token.lexeme + "'");
	}

	return { std::string(terminal), true };
}
