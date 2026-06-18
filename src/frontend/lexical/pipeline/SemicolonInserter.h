#pragma once

#include "src/frontend/lexical/core/Token.h"

#include <vector>

class SemicolonInserter
{
public:
	[[nodiscard]] static std::vector<Token> Insert(const std::vector<Token>& tokens);

private:
	static void UpdateDelimiterDepth(TokenType type, int& parenDepth, int& bracketDepth);
	static void AppendProcessedToken(
		const Token& token,
		const std::vector<Token>& tokens,
		size_t index,
		int parenDepth,
		int bracketDepth,
		std::vector<Token>& result);
	[[nodiscard]] static bool ShouldInsertSemicolonAtNewline(
		const std::vector<Token>& tokens,
		size_t newlineIndex,
		int parenDepth,
		int bracketDepth,
		const std::vector<Token>& result);

	[[nodiscard]] static bool ShouldInsertSemicolonAfter(TokenType type);
	[[nodiscard]] static bool IsLineContinuation(TokenType type);
	[[nodiscard]] static bool IsNextMeaningfulTokenElse(
		const std::vector<Token>& tokens,
		size_t newlineIndex);
	[[nodiscard]] static bool IsNewlineToken(const Token& token);
	[[nodiscard]] static bool IsElseToken(const Token& token);

	[[nodiscard]] static bool IsOpeningToken(TokenType type);
	[[nodiscard]] static bool IsClosingToken(TokenType type);
};
