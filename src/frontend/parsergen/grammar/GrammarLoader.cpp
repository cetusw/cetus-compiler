#include "GrammarLoader.h"
#include "GrammarLineParser.h"
#include "src/support/io/FileReader.h"

constexpr char COMMENT_CHAR = '#';

Grammar GrammarLoader::LoadFromFile(const std::string& path)
{
	return LoadFromLines(FileReader::ReadLines(path));
}

Grammar GrammarLoader::LoadFromLines(const std::vector<std::string>& lines)
{
	Grammar grammar;
	bool isFirstRule = true;

	for (const std::string& line : lines)
	{
		if (line.empty() || line[0] == COMMENT_CHAR)
		{
			continue;
		}

		const std::string startSymbolValue = ParseLine(line, grammar);

		if (isFirstRule && !startSymbolValue.empty())
		{
			grammar.SetStartSymbol(Symbol(startSymbolValue, false));
			isFirstRule = false;
		}
	}

	return grammar;
}

std::string GrammarLoader::ParseLine(const std::string& line, Grammar& grammar)
{
	const ParsedGrammarLine parsed = GrammarLineParser::Parse(line);
	AddProductions(grammar, parsed);
	return parsed.lhs.GetValue();
}

void GrammarLoader::AddProductions(Grammar& grammar, const ParsedGrammarLine& line)
{
	std::vector<Symbol> rhs;

	for (const std::string& token : line.rhsTokens)
	{
		if (token == "|")
		{
			grammar.AddRule(line.lhs, rhs, line.semanticTag);
			rhs.clear();
			continue;
		}

		if (token == "e" || token == "ε")
		{
			continue;
		}

		rhs.push_back(GrammarLineParser::ParseSymbolToken(token));
	}

	grammar.AddRule(line.lhs, rhs, line.semanticTag);
}
