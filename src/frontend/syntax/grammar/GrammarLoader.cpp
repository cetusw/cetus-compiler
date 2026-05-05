#include "GrammarLoader.h"
#include <fstream>
#include <sstream>

constexpr char COMMENT_CHAR = '#';
constexpr char SEMANTIC_TAG_CHAR = '@';

Grammar GrammarLoader::LoadFromFile(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open grammar file: " + path);
	}

	return ParseFile(std::move(file));
}

Grammar GrammarLoader::ParseFile(std::ifstream file)
{
	Grammar grammar;
	std::string line;
	bool isFirstRule = true;

	while (std::getline(file, line))
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
	const ParsedLine parsed = ParseProductionLine(line);
	AddProductions(grammar, parsed);
	return parsed.lhs.GetValue();
}

GrammarLoader::ParsedLine GrammarLoader::ParseProductionLine(const std::string& line)
{
	std::stringstream lineStream(line);
	std::string lhsValue;
	std::string arrow;

	if (!(lineStream >> lhsValue >> arrow))
	{
		throw std::runtime_error("Failed to parse grammar line: " + line);
	}

	const Symbol lhsSymbol = ParseSymbolToken(lhsValue);
	if (lhsSymbol.IsTerminal())
	{
		throw std::runtime_error("lhs must be a non-terminal wrapped in '~': " + lhsValue);
	}

	ParsedLine parsed{ lhsSymbol, {}, {} };
	std::string token;
	while (lineStream >> token)
	{
		if (!token.empty() && token.front() == SEMANTIC_TAG_CHAR)
		{
			if (parsed.hasSemanticTag)
			{
				throw std::runtime_error("Multiple semantic tags in grammar line: " + line);
			}
			parsed.semanticTag = ParseSemanticTagName(token.substr(1));
			parsed.hasSemanticTag = true;
			continue;
		}

		parsed.rhs.push_back(token);
	}

	return parsed;
}

void GrammarLoader::AddProductions(Grammar& grammar, const ParsedLine& line)
{
	std::vector<Symbol> rhs;

	for (const std::string& token : line.rhs)
	{
		if (token == "|")
		{
			if (line.hasSemanticTag)
			{
				throw std::runtime_error("Tagged grammar alternatives must be split into separate lines.");
			}
			grammar.AddRule(line.lhs, rhs, line.semanticTag);
			rhs.clear();
			continue;
		}

		if (token == "e" || token == "ε")
		{
			continue;
		}

		rhs.push_back(ParseSymbolToken(token));
	}

	grammar.AddRule(line.lhs, rhs, line.semanticTag);
}

bool GrammarLoader::IsNonTerminal(const std::string& value)
{
	return value.size() >= 2 && value.front() == '~' && value.back() == '~';
}

Symbol GrammarLoader::ParseSymbolToken(const std::string& token)
{
	if (token.empty())
	{
		throw std::runtime_error("Encountered empty grammar symbol.");
	}

	if (!IsNonTerminal(token))
	{
		return { token, true };
	}

	const std::string symbolValue = token.substr(1, token.size() - 2);
	if (symbolValue.empty())
	{
		throw std::runtime_error("Non-terminal wrapper '~' must contain a symbol name.");
	}

	return { symbolValue, false };
}
