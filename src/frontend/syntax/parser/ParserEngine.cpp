#include "ParserEngine.h"

#include "TokenSymbolMapper.h"
#include "src/frontend/ast/building/AstReductionBuilder.h"
#include "src/frontend/syntax/diagnostics/SyntaxErrorBuilder.h"
#include <stdexcept>

ParserEngine::ParserEngine(const PreparedGrammar& preparedGrammar)
	: m_preparedGrammar(preparedGrammar)
{
}

ParseResult ParserEngine::Parse(const std::vector<Token>& tokens) const
{
	if (tokens.empty())
	{
		return SyntaxErrorBuilder::EmptyTokenStream();
	}

	ParseContext ctx;
	while (ctx.tokenIndex < tokens.size())
	{
		const Token& token = tokens[ctx.tokenIndex];
		const Symbol symbol = TokenSymbolMapper::MapTokenToGrammarSymbol(token);
		const Action action = GetAction(ctx.stack.CurrentState(), symbol);
		if (auto result = HandleAction(ctx, action, token))
		{
			return std::move(*result);
		}
	}

	return SyntaxErrorBuilder::UnexpectedEndOfStream(tokens.back());
}

std::optional<ParseResult> ParserEngine::HandleAction(
	ParseContext& ctx,
	const Action& action,
	const Token& token) const
{
	switch (action.type)
	{
	case ActionType::SHIFT:
		return Shift(ctx, action, token);
	case ActionType::REDUCE:
		return Reduce(ctx, token, action.value);
	case ActionType::ACCEPT:
		return Accept(ctx, token);
	case ActionType::ERROR:
	case ActionType::GOTO:
		return SyntaxErrorBuilder(m_preparedGrammar).UnexpectedToken(ctx.stack.CurrentState(), token);
	}

	return SyntaxErrorBuilder(m_preparedGrammar).UnexpectedToken(ctx.stack.CurrentState(), token);
}

std::optional<ParseResult> ParserEngine::Shift(
	ParseContext& ctx,
	const Action& action,
	const Token& token)
{
	ctx.stack.PushSemanticValue(BuildShiftValue(token));
	ctx.stack.PushState(action.value);
	++ctx.tokenIndex;
	return std::nullopt;
}

std::optional<ParseResult> ParserEngine::Reduce(
	ParseContext& ctx,
	const Token& token,
	const int ruleIndex) const
{
	const ParserRule& rule = GetRule(ruleIndex);
	if (auto error = ValidateReduceContext(ctx.stack, rule.rhs.size(), token.line))
	{
		return error;
	}

	std::vector<AstSemanticValue> values = ctx.stack.PopSemanticValues(rule.rhs.size());
	ctx.stack.PopStates(rule.rhs.size());

	const Action actionAfterReduce = GetAction(ctx.stack.CurrentState(), rule.lhs);
	if (auto error = ValidateGotoAfterReduce(actionAfterReduce.type, token.line))
	{
		return error;
	}

	ApplyReducedSemanticValue(ctx.stack, rule, std::move(values));
	ctx.stack.PushState(actionAfterReduce.value);
	return std::nullopt;
}

ParseResult ParserEngine::Accept(ParseContext& ctx, const Token& token)
{
	try
	{
		return ParseResult::Success(
			token.line,
			AstReductionBuilder::TakeProgram(ctx.stack.TopSemanticValue()));
	}
	catch (const std::logic_error&)
	{
		return ParseResult::Error(
			DiagnosticStage::Internal,
			token.line,
			"Internal parser error: Program AST was not produced for accepted input.");
	}
}

std::optional<ParseResult> ParserEngine::ValidateReduceContext(
	const ParserStack& stack,
	const std::size_t rhsSize,
	const int line)
{
	if (!stack.CanReduce(rhsSize))
	{
		return ParseResult::Error(
			DiagnosticStage::Syntax,
			line,
			"Parser stack underflow during reduce.");
	}
	return std::nullopt;
}

std::optional<ParseResult> ParserEngine::ValidateGotoAfterReduce(
	const ActionType actionType,
	const int line)
{
	if (actionType != ActionType::GOTO)
	{
		return ParseResult::Error(
			DiagnosticStage::Syntax,
			line,
			"Missing goto action after reduce.");
	}
	return std::nullopt;
}

void ParserEngine::ApplyReducedSemanticValue(
	ParserStack& stack,
	const ParserRule& rule,
	std::vector<AstSemanticValue> values)
{
	AstSemanticValue newSemanticValue = AstReductionBuilder::Build(rule, std::move(values));
	stack.PushSemanticValue(std::move(newSemanticValue));
}

AstSemanticValue ParserEngine::BuildShiftValue(const Token& token)
{
	return TokenValue{ token };
}

const ParserRule& ParserEngine::GetRule(const int ruleIndex) const
{
	if (ruleIndex < 0 || ruleIndex >= static_cast<int>(m_preparedGrammar.rules.size()))
	{
		throw std::runtime_error("Parser rule index is out of range: " + std::to_string(ruleIndex));
	}

	return m_preparedGrammar.rules[ruleIndex];
}

Action ParserEngine::GetAction(const int state, const Symbol& symbol) const
{
	const auto stateIt = m_preparedGrammar.table.find(state);
	if (stateIt == m_preparedGrammar.table.end())
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
