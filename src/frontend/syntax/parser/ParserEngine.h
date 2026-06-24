#pragma once

#include "ParseResult.h"
#include "ParserStack.h"
#include "src/frontend/lexical/core/Token.h"
#include "src/frontend/syntax/table/PreparedGrammar.h"
#include <optional>

class ParserEngine
{
public:
	explicit ParserEngine(const PreparedGrammar& preparedGrammar);

	[[nodiscard]] ParseResult Parse(const std::vector<Token>& tokens) const;

private:
	struct ParseContext
	{
		ParserStack stack;
		std::size_t tokenIndex = 0;
	};

	[[nodiscard]] std::optional<ParseResult> HandleAction(
		ParseContext& ctx,
		const Action& action,
		const Token& token) const;
	[[nodiscard]] static std::optional<ParseResult> Shift(
		ParseContext& ctx,
		const Action& action,
		const Token& token);
	[[nodiscard]] std::optional<ParseResult> Reduce(
		ParseContext& ctx,
		const Token& token,
		int ruleIndex) const;
	[[nodiscard]] static ParseResult Accept(ParseContext& ctx, const Token& token);
	[[nodiscard]] static std::optional<ParseResult> ValidateReduceContext(
		const ParserStack& stack,
		std::size_t rhsSize,
		int line);
	[[nodiscard]] static std::optional<ParseResult> ValidateGotoAfterReduce(
		ActionType actionType,
		int line);
	static void ApplyReducedSemanticValue(
		ParserStack& stack,
		const ParserRule& rule,
		std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildShiftValue(const Token& token);
	[[nodiscard]] const ParserRule& GetRule(int ruleIndex) const;
	[[nodiscard]] Action GetAction(int state, const Symbol& symbol) const;

	const PreparedGrammar& m_preparedGrammar;
};
