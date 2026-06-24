#pragma once

#include "parser/ParseResult.h"
#include "src/frontend/lexical/core/Token.h"
#include "src/frontend/syntax/table/PreparedGrammar.h"
#include <vector>

class SyntaxAnalyzer
{
public:
	explicit SyntaxAnalyzer(PreparedGrammar preparedGrammar);

	[[nodiscard]] ParseResult Analyze(const std::vector<Token>& tokens) const;

private:
	PreparedGrammar m_preparedGrammar;
};
