#include "TokenPostProcessor.h"

void TokenPostProcessor::Process(std::vector<Token>& tokens)
{
	PromoteStructLiteralTypeHeads(tokens);
}

void TokenPostProcessor::PromoteStructLiteralTypeHeads(std::vector<Token>& tokens)
{
	const std::unordered_set<std::string> declaredTypeNames = CollectDeclaredTypeNames(tokens);
	for (std::size_t index = 0; index + 1 < tokens.size(); ++index)
	{
		Token& token = tokens[index];
		if (token.type != TokenType::IDENTIFIER)
		{
			continue;
		}
		if (!declaredTypeNames.contains(token.lexeme))
		{
			continue;
		}
		if (tokens[index + 1].type != TokenType::LBRACE)
		{
			continue;
		}
		if (!IsStructLiteralHeadContext(tokens, index))
		{
			continue;
		}

		token.type = TokenType::TYPE_IDENTIFIER;
	}
}

std::unordered_set<std::string> TokenPostProcessor::CollectDeclaredTypeNames(const std::vector<Token>& tokens)
{
	std::unordered_set<std::string> typeNames;
	for (std::size_t index = 0; index + 1 < tokens.size(); ++index)
	{
		if (tokens[index].type == TokenType::TYPE && tokens[index + 1].type == TokenType::IDENTIFIER)
		{
			typeNames.insert(tokens[index + 1].lexeme);
		}
	}

	return typeNames;
}

bool TokenPostProcessor::IsStructLiteralHeadContext(
	const std::vector<Token>& tokens,
	const std::size_t tokenIndex)
{
	if (tokenIndex == 0)
	{
		return false;
	}

	switch (tokens[tokenIndex - 1].type)
	{
	case TokenType::BIT_AND:
	case TokenType::RETURN:
	case TokenType::EQUAL:
	case TokenType::COLON_EQUAL:
	case TokenType::COMMA:
	case TokenType::LPAREN:
	case TokenType::LBRACE:
	case TokenType::COLON:
	case TokenType::SEMICOLON:
	case TokenType::PLUS:
	case TokenType::MINUS:
	case TokenType::SLASH:
	case TokenType::PERCENT:
	case TokenType::OR_OR:
	case TokenType::AND_AND:
	case TokenType::EQUAL_EQUAL:
	case TokenType::BANG_EQUAL:
	case TokenType::LESS:
	case TokenType::LESS_EQUAL:
	case TokenType::GREATER:
	case TokenType::GREATER_EQUAL:
		return true;
	default:
		return false;
	}
}
