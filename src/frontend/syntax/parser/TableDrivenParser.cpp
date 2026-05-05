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
		return ParseResult::Error(1, "Token stream is empty.");
	}

	ParseContext ctx;
	while (ctx.tokenIndex < tokens.size())
	{
		const Token& token = tokens[ctx.tokenIndex];
		const Symbol symbol = TokenSymbolMapper::MapTokenToGrammarSymbol(token);
		const Action action = GetAction(ctx.stateStack.back(), symbol);
		if (auto result = HandleAction(ctx, action, token))
		{
			return std::move(*result);
		}
	}

	return ParseResult::Error(tokens.back().line, "Unexpected end of token stream.");
}

std::optional<ParseResult> TableDrivenParser::HandleAction(
	ParseContext& ctx,
	const Action& action,
	const Token& token) const
{
	switch (action.type)
	{
	case ActionType::SHIFT:
		ctx.semanticStack.push_back(BuildShiftValue(token));
		ctx.stateStack.push_back(action.value);
		++ctx.tokenIndex;
		return std::nullopt;
	case ActionType::REDUCE:
		return Reduce(ctx, token, action.value);
	case ActionType::ACCEPT:
		return BuildSuccessResult(ctx, token);
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
	if (auto error = ValidateReduceContext(ctx, rule.rhs.size(), token.line))
	{
		return error;
	}

	std::vector<AstSemanticValue> values = PopSemanticValues(ctx, rule.rhs.size());
	PopStates(ctx, rule.rhs.size());

	const Action actionAfterReduce = GetAction(ctx.stateStack.back(), rule.lhs);
	if (auto error = ValidateGotoAfterReduce(actionAfterReduce.type, token.line))
	{
		return error;
	}

	ApplyReducedSemanticValue(ctx, rule, std::move(values));
	ctx.stateStack.push_back(actionAfterReduce.value);
	return std::nullopt;
}

std::optional<ParseResult> TableDrivenParser::ValidateReduceContext(
	const ParseContext& ctx,
	const size_t rhsSize,
	const int line)
{
	if (ctx.stateStack.size() <= rhsSize)
	{
		return ParseResult::Error(line, "Parser state stack underflow during reduce.");
	}
	if (ctx.semanticStack.size() < rhsSize)
	{
		return ParseResult::Error(line, "Parser semantic stack underflow during reduce.");
	}
	return std::nullopt;
}

void TableDrivenParser::PopStates(ParseContext& ctx, const std::size_t count)
{
	for (std::size_t i = 0; i < count; ++i)
	{
		ctx.stateStack.pop_back();
	}
}

std::optional<ParseResult> TableDrivenParser::ValidateGotoAfterReduce(
	const ActionType actionType,
	const int line)
{
	if (actionType != ActionType::GOTO)
	{
		return ParseResult::Error(line, "Missing goto action after reduce.");
	}
	return std::nullopt;
}

void TableDrivenParser::ApplyReducedSemanticValue(
	ParseContext& ctx,
	const ParserRule& rule,
	std::vector<AstSemanticValue> values)
{
	AstSemanticValue newSemanticValue = AstReductionBuilder::Build(rule, std::move(values));
	ctx.semanticStack.push_back(std::move(newSemanticValue));
}

ParseResult TableDrivenParser::BuildSuccessResult(ParseContext& ctx, const Token& token)
{
	if (ctx.semanticStack.empty() || !ctx.semanticStack.back().expr)
	{
		return ParseResult::Error(token.line, "Internal parser error: AST was not produced for accepted input.");
	}
	return ParseResult::Success(token.line, std::move(ctx.semanticStack.back().expr));
}

AstSemanticValue TableDrivenParser::BuildShiftValue(const Token& token)
{
	return { nullptr, token };
}

std::vector<AstSemanticValue> TableDrivenParser::PopSemanticValues(
	ParseContext& ctx,
	const std::size_t count)
{
	std::vector<AstSemanticValue> values(count);
	for (std::size_t i = 0; i < count; ++i)
	{
		values[count - 1 - i] = std::move(ctx.semanticStack.back());
		ctx.semanticStack.pop_back();
	}
	return values;
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
	std::vector<std::string> expectedTerminals = FindExpectedTerminal(state);
	return ParseResult::Error(token.line, CreateMessage(token, expectedTerminals), std::move(expectedTerminals));
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
