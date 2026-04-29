#pragma once

#include "ParseResult.h"
#include "src/common/types/Token.h"
#include "src/compiler/lalr/types/ParserDefinition.h"
#include <optional>
#include <vector>

class TableDrivenParser
{
public:
	explicit TableDrivenParser(ParserDefinition definition);

	[[nodiscard]] ParseResult Parse(const std::vector<Token>& tokens) const;

private:
	struct ParseContext
	{
		std::vector<int> stateStack = { 0 };
		std::size_t tokenIndex = 0;
	};

	[[nodiscard]] std::optional<ParseResult> HandleAction(
		ParseContext& ctx,
		const Action& action,
		const Token& token) const;
	[[nodiscard]] std::optional<ParseResult> Reduce(
		ParseContext& ctx,
		const Token& token,
		int ruleIndex) const;
	[[nodiscard]] static ParseResult BuildSuccessResult(const Token& token);
	[[nodiscard]] const ParserRule& GetRule(int ruleIndex) const;
	[[nodiscard]] Action GetAction(int state, const Symbol& symbol) const;
	[[nodiscard]] ParseResult BuildUnexpectedTokenResult(int state, const Token& token) const;
	[[nodiscard]] std::vector<std::string> FindExpectedTerminal(int state) const;
	[[nodiscard]] static std::string CreateMessage(const Token& token, const std::vector<std::string>& expectedTerminals);
	static std::string DescribeToken(const Token& token);

	ParserDefinition m_definition;
};
