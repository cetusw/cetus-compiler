#include "TableDrivenParser.h"

#include "TokenSymbolMapper.h"
#include <sstream>
#include <stdexcept>

TableDrivenParser::TableDrivenParser(ParserDefinition definition)
	: m_definition(std::move(definition))
{
}

ParseResult TableDrivenParser::Parse(const std::vector<Token>& tokens) const
{
	if (tokens.empty())
	{
		return { false, 1, "Token stream is empty.", {} };
	}

	ParseContext ctx;
	while (ctx.tokenIndex < tokens.size())
	{
		const Token& token = tokens[ctx.tokenIndex];
		const Symbol symbol = TokenSymbolMapper::MapTokenToGrammarSymbol(token);
		const int currentState = ctx.stateStack.back();
		const Action action = GetAction(currentState, symbol);
		if (const auto result = HandleAction(ctx, action, token))
		{
			return *result;
		}
	}

	return { false, tokens.back().line, "Unexpected end of token stream.", {} };
}

std::optional<ParseResult> TableDrivenParser::HandleAction(
	ParseContext& ctx,
	const Action& action,
	const Token& token) const
{
	switch (action.type)
	{
	case ActionType::SHIFT:
		ctx.stateStack.push_back(action.value);
		++ctx.tokenIndex;
		return std::nullopt;
	case ActionType::REDUCE:
		return Reduce(ctx, token, action.value);
	case ActionType::ACCEPT:
		return BuildSuccessResult(token);
	case ActionType::ERROR:
	case ActionType::GOTO:
		return BuildUnexpectedTokenResult(ctx.stateStack.back(), token);
	}

	return BuildUnexpectedTokenResult(ctx.stateStack.back(), token);
}

std::optional<ParseResult> TableDrivenParser::Reduce(
	ParseContext& ctx,
	const Token& token,
	const int ruleIndex) const
{
	const ParserRule& rule = GetRule(ruleIndex);
	if (ctx.stateStack.size() <= rule.rhs.size())
	{
		return ParseResult{ false, token.line, "Parser stack underflow during reduce.", {} };
	}

	for (std::size_t i = 0; i < rule.rhs.size(); ++i)
	{
		ctx.stateStack.pop_back();
	}

	const Action gotoAction = GetAction(ctx.stateStack.back(), rule.lhs);
	if (gotoAction.type != ActionType::GOTO)
	{
		return ParseResult{ false, token.line, "Missing goto action after reduce.", {} };
	}

	ctx.stateStack.push_back(gotoAction.value);
	return std::nullopt;
}

ParseResult TableDrivenParser::BuildSuccessResult(const Token& token)
{
	return { true, token.line, "Syntax analysis completed successfully.", {} };
}

const ParserRule& TableDrivenParser::GetRule(const int ruleIndex) const
{
	if (ruleIndex < 0 || ruleIndex >= static_cast<int>(m_definition.rules.size()))
	{
		throw std::runtime_error("Parser rule index is out of range: " + std::to_string(ruleIndex));
	}

	return m_definition.rules[ruleIndex];
}

Action TableDrivenParser::GetAction(const int state, const Symbol& symbol) const
{
	const auto stateIt = m_definition.table.find(state);
	if (stateIt == m_definition.table.end())
	{
		return {};
	}

	const auto actionIt = stateIt->second.find(symbol);
	if (actionIt == stateIt->second.end())
	{
		return {};
	}

	return actionIt->second;
}

ParseResult TableDrivenParser::BuildUnexpectedTokenResult(const int state, const Token& token) const
{
	ParseResult result;
	result.success = false;
	result.line = token.line;
	result.expectedTerminals = FindExpectedTerminal(state);
	result.message = CreateMessage(token, result.expectedTerminals);
	return result;
}

std::vector<std::string> TableDrivenParser::FindExpectedTerminal(const int state) const
{
	std::vector<std::string> expected;
	const auto stateIt = m_definition.table.find(state);
	if (stateIt != m_definition.table.end())
	{
		for (const auto& [symbol, action] : stateIt->second)
		{
			if (symbol.IsTerminal() && action.type != ActionType::ERROR)
			{
				expected.push_back(symbol.GetValue());
			}
		}
	}

	return expected;
}

std::string TableDrivenParser::CreateMessage(const Token& token, const std::vector<std::string>& expectedTerminals)
{
	std::ostringstream message;
	message << "Syntax error at line " << token.line << ": unexpected token " << DescribeToken(token);
	if (!expectedTerminals.empty())
	{
		message << ". Expected: ";
		for (std::size_t index = 0; index < expectedTerminals.size(); ++index)
		{
			if (index > 0)
			{
				message << ", ";
			}
			message << expectedTerminals[index];
		}
	}

	return message.str();
}

std::string TableDrivenParser::DescribeToken(const Token& token)
{
	if (!token.lexeme.empty())
	{
		return "'" + token.lexeme + "'";
	}

	return "at end of input";
}
