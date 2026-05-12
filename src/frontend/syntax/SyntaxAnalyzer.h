#pragma once

#include "parser/ParseResult.h"
#include <string>
#include <vector>

class SyntaxAnalyzer
{
public:
	explicit SyntaxAnalyzer(std::string grammarPath = "docs/cetus_grammar.txt");

	[[nodiscard]] ParseResult Analyze(const std::string& sourceFilePath) const;

private:
	[[nodiscard]] ParseResult ParseSource(const std::string& source) const;

	std::string m_grammarPath;
};
