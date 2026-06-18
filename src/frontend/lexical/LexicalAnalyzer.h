#pragma once

#include "core/Token.h"

#include <optional>
#include <string>
#include <vector>

struct LexerResult
{
	std::vector<Token> tokens;
	std::optional<std::string> error;
	int errorLine = 1;
};

class LexicalAnalyzer
{
public:
	explicit LexicalAnalyzer(std::string source);
	[[nodiscard]] LexerResult ScanTokens() const;

private:
	std::string m_source;
};
