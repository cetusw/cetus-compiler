#include "SemicolonInserter.h"

#include <algorithm>

std::vector<Token> SemicolonInserter::Insert(const std::vector<Token>& tokens)
{
	std::vector<Token> result;
	result.reserve(tokens.size());

	int parenDepth = 0;
	int bracketDepth = 0;

	for (std::size_t index = 0; index < tokens.size(); ++index)
	{
		const Token& token = tokens[index];
		UpdateDelimiterDepth(token.type, parenDepth, bracketDepth);
		AppendProcessedToken(token, tokens, index, parenDepth, bracketDepth, result);
	}

	return result;
}

void SemicolonInserter::UpdateDelimiterDepth(
	const TokenType type,
	int& parenDepth,
	int& bracketDepth)
{
	if (IsOpeningToken(type))
	{
		type == TokenType::LPAREN ? ++parenDepth : ++bracketDepth;
		return;
	}

	if (!IsClosingToken(type))
	{
		return;
	}

	if (type == TokenType::RPAREN)
	{
		parenDepth = std::max(0, parenDepth - 1);
		return;
	}

	bracketDepth = std::max(0, bracketDepth - 1);
}

void SemicolonInserter::AppendProcessedToken(
	const Token& token,
	const std::vector<Token>& tokens,
	const std::size_t index,
	const int parenDepth,
	const int bracketDepth,
	std::vector<Token>& result)
{
	if (!IsNewlineToken(token))
	{
		result.push_back(token);
		return;
	}

	if (ShouldInsertSemicolonAtNewline(tokens, index, parenDepth, bracketDepth, result))
	{
		result.push_back({ TokenType::SEMICOLON, ";", token.line });
	}
}

bool SemicolonInserter::ShouldInsertSemicolonAtNewline(
	const std::vector<Token>& tokens,
	const std::size_t newlineIndex,
	const int parenDepth,
	const int bracketDepth,
	const std::vector<Token>& result)
{
	if (result.empty() || parenDepth > 0 || bracketDepth > 0)
	{
		return false;
	}

	const TokenType previousType = result.back().type;
	return !IsLineContinuation(previousType)
		&& ShouldInsertSemicolonAfter(previousType)
		&& !IsNextMeaningfulTokenElse(tokens, newlineIndex);
}

bool SemicolonInserter::ShouldInsertSemicolonAfter(const TokenType type)
{
	switch (type)
	{
	case TokenType::IDENTIFIER:
	case TokenType::TYPE_IDENTIFIER:
	case TokenType::STRING:
	case TokenType::INT_LIT:
	case TokenType::FLOAT_LIT:
	case TokenType::TRUE:
	case TokenType::FALSE:
	case TokenType::NIL:
	case TokenType::RPAREN:
	case TokenType::RBRACKET:
	case TokenType::RBRACE:
	case TokenType::BREAK:
	case TokenType::CONTINUE:
	case TokenType::RETURN:
	case TokenType::PLUS_PLUS:
	case TokenType::MINUS_MINUS:
		return true;
	default:
		return false;
	}
}

bool SemicolonInserter::IsLineContinuation(const TokenType type)
{
	switch (type)
	{
	case TokenType::COMMA:
	case TokenType::DOT:
	case TokenType::PLUS:
	case TokenType::MINUS:
	case TokenType::STAR:
	case TokenType::SLASH:
	case TokenType::PERCENT:
	case TokenType::EQUAL:
	case TokenType::COLON_EQUAL:
	case TokenType::AND_AND:
	case TokenType::OR_OR:
	case TokenType::LESS:
	case TokenType::LESS_EQUAL:
	case TokenType::GREATER:
	case TokenType::GREATER_EQUAL:
	case TokenType::EQUAL_EQUAL:
	case TokenType::BANG_EQUAL:
		return true;
	default:
		return false;
	}
}

bool SemicolonInserter::IsNextMeaningfulTokenElse(
	const std::vector<Token>& tokens,
	const std::size_t newlineIndex)
{
	for (std::size_t index = newlineIndex + 1; index < tokens.size(); ++index)
	{
		if (IsNewlineToken(tokens[index]))
		{
			continue;
		}

		return IsElseToken(tokens[index]);
	}

	return false;
}

bool SemicolonInserter::IsNewlineToken(const Token& token)
{
	return token.type == TokenType::NEWLINE;
}

bool SemicolonInserter::IsElseToken(const Token& token)
{
	return token.type == TokenType::ELSE;
}

bool SemicolonInserter::IsOpeningToken(const TokenType type)
{
	return type == TokenType::LPAREN || type == TokenType::LBRACKET;
}

bool SemicolonInserter::IsClosingToken(const TokenType type)
{
	return type == TokenType::RPAREN || type == TokenType::RBRACKET;
}
