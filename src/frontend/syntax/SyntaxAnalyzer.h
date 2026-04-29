#pragma once

#include "parser/ParseResult.h"
#include "src/frontend/lexer/Token.h"
#include <string>
#include <vector>

class SyntaxAnalyzer
{
public:
	explicit SyntaxAnalyzer(std::string grammarPath = "docs/cetus_grammar.txt");

	[[nodiscard]] ParseResult ParseFile(const std::string& sourceFilePath) const;
	[[nodiscard]] ParseResult ParseSource(const std::string& source) const;

private:
	static void ValidateTokens(const std::vector<Token>& tokens);

	std::string m_grammarPath;
};
