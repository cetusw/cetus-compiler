#pragma once

#include "src/frontend/lexical/core/Token.h"

#include <string_view>
#include <unordered_map>

class KeywordClassifier
{
public:
	[[nodiscard]] static TokenType Classify(std::string_view lexeme);

private:
	static const std::unordered_map<std::string_view, TokenType> m_keywords;
};
