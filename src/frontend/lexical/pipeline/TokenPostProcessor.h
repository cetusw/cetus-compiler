#pragma once

#include "src/frontend/lexical/core/Token.h"

#include <unordered_set>
#include <vector>

class TokenPostProcessor
{
public:
	static void Process(std::vector<Token>& tokens);

private:
	static void PromoteStructLiteralTypeHeads(std::vector<Token>& tokens);
	[[nodiscard]] static std::unordered_set<std::string> CollectDeclaredTypeNames(
		const std::vector<Token>& tokens);
	[[nodiscard]] static bool IsStructLiteralHeadContext(
		const std::vector<Token>& tokens,
		std::size_t tokenIndex);
};
