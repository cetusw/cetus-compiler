#include "SyntaxAnalyzer.h"

#include "parser/ParserEngine.h"

SyntaxAnalyzer::SyntaxAnalyzer(PreparedGrammar preparedGrammar)
	: m_preparedGrammar(std::move(preparedGrammar))
{
}

ParseResult SyntaxAnalyzer::Analyze(const std::vector<Token>& tokens) const
{
	const ParserEngine engine(m_preparedGrammar);
	return engine.Parse(tokens);
}
