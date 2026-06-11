#include "TokenSymbolMapper.h"

#include <stdexcept>
#include <string_view>

namespace
{
std::string_view TokenTypeToGrammarTerminal(const TokenType type)
{
	switch (type)
	{
	case TokenType::OR_OR:
		return "OR_OR";
	case TokenType::AND_AND:
		return "AND_AND";
	case TokenType::BIT_AND:
		return "BIT_AND";
	case TokenType::SEMICOLON:
		return "SEMICOLON";
	case TokenType::IF:
		return "IF";
	case TokenType::ELSE:
		return "ELSE";
	case TokenType::FOR:
		return "FOR";
	case TokenType::BREAK:
		return "BREAK";
	case TokenType::CONTINUE:
		return "CONTINUE";
	case TokenType::RETURN:
		return "RETURN";
	case TokenType::FUNC:
		return "FUNC";
	case TokenType::TYPE:
		return "TYPE";
	case TokenType::VAR:
		return "VAR";
	case TokenType::STRUCT:
		return "STRUCT";
	case TokenType::COMMA:
		return "COMMA";
	case TokenType::EQUAL:
		return "EQUAL";
	case TokenType::COLON_EQUAL:
		return "COLON_EQUAL";
	case TokenType::BANG:
		return "BANG";
	case TokenType::TRUE:
		return "TRUE";
	case TokenType::FALSE:
		return "FALSE";
	case TokenType::PLUS:
		return "PLUS";
	case TokenType::PLUS_PLUS:
		return "PLUS_PLUS";
	case TokenType::MINUS:
		return "MINUS";
	case TokenType::MINUS_MINUS:
		return "MINUS_MINUS";
	case TokenType::STAR:
		return "STAR";
	case TokenType::SLASH:
		return "SLASH";
	case TokenType::PERCENT:
		return "PERCENT";
	case TokenType::LPAREN:
		return "LPAREN";
	case TokenType::RPAREN:
		return "RPAREN";
	case TokenType::LBRACE:
		return "LBRACE";
	case TokenType::RBRACE:
		return "RBRACE";
	case TokenType::LBRACKET:
		return "LBRACKET";
	case TokenType::RBRACKET:
		return "RBRACKET";
	case TokenType::DOT:
		return "DOT";
	case TokenType::LESS:
		return "LESS";
	case TokenType::LESS_EQUAL:
		return "LESS_EQUAL";
	case TokenType::BANG_EQUAL:
		return "BANG_EQUAL";
	case TokenType::EQUAL_EQUAL:
		return "EQUAL_EQUAL";
	case TokenType::GREATER:
		return "GREATER";
	case TokenType::GREATER_EQUAL:
		return "GREATER_EQUAL";
	case TokenType::IDENTIFIER:
		return "IDENTIFIER";
	case TokenType::INT_LIT:
		return "INT_LIT";
	case TokenType::FLOAT_LIT:
		return "FLOAT_LIT";
	case TokenType::STRING:
		return "STRING";
	case TokenType::EOF_TOKEN:
		return "⊥";
	default:
		return "";
	}
}
} // namespace

Symbol TokenSymbolMapper::MapTokenToGrammarSymbol(const Token& token)
{
	const std::string_view terminal = TokenTypeToGrammarTerminal(token.type);
	if (terminal.empty())
	{
		throw std::runtime_error("Token is not supported by the current grammar: '" + token.lexeme + "'");
	}

	return { std::string(terminal), true };
}
