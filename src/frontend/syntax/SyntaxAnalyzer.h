#pragma once

#include "parser/AstReductionBuilder.h"
#include "parser/ParseResult.h"
#include "src/frontend/syntax/lalr/types/PreparedGrammar.h"
#include <vector>

class SyntaxAnalyzer
{
public:
	explicit SyntaxAnalyzer(PreparedGrammar preparedGrammar);

	[[nodiscard]] ParseResult Analyze(const std::vector<Token>& tokens) const;

private:
	struct SyntaxContext
	{
		std::vector<int> stateStack = { 0 };
		std::vector<AstSemanticValue> semanticStack;
		std::size_t tokenIndex = 0;
	};

	[[nodiscard]] std::optional<ParseResult> HandleAction(
		SyntaxContext& ctx,
		const Action& action,
		const Token& token) const;
	[[nodiscard]] std::optional<ParseResult> Reduce(
		SyntaxContext& ctx,
		const Token& token,
		int ruleIndex) const;
	[[nodiscard]] static std::optional<ParseResult> ValidateReduceContext(
		const SyntaxContext& ctx,
		size_t rhsSize, int line);
	static void PopStates(SyntaxContext& ctx, std::size_t count);
	[[nodiscard]] static std::optional<ParseResult> ValidateGotoAfterReduce(
		ActionType actionType, int line);
	static void ApplyReducedSemanticValue(
		SyntaxContext& ctx,
		const ParserRule& rule,
		std::vector<AstSemanticValue> values);
	[[nodiscard]] static ParseResult BuildSuccessResult(SyntaxContext& ctx, const Token& token);
	[[nodiscard]] static AstSemanticValue BuildShiftValue(const Token& token);
	[[nodiscard]] static std::vector<AstSemanticValue> PopSemanticValues(SyntaxContext& ctx, std::size_t count);
	[[nodiscard]] const ParserRule& GetRule(int ruleIndex) const;
	[[nodiscard]] Action GetAction(int state, const Symbol& symbol) const;
	[[nodiscard]] ParseResult BuildUnexpectedTokenResult(int state, const Token& token) const;
	[[nodiscard]] std::vector<std::string> FindExpectedTerminal(int state) const;
	[[nodiscard]] static std::string CreateMessage(const Token& token, const std::vector<std::string>& expectedTerminals);
	static std::string DescribeToken(const Token& token);

	PreparedGrammar m_preparedGrammar;
};
