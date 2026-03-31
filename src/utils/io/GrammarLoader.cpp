#include "GrammarLoader.h"
#include <fstream>
#include <sstream>

Grammar GrammarLoader::LoadFromFile(const std::string& path)
{
	Grammar grammar;
	std::ifstream file(path);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open grammar file: " + path);
	}

	std::string line;
	bool isFirstRule = true;

	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
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
	std::stringstream lineStream(line);
	std::string leftHandSideValue;
	std::string arrow;

	if (!(lineStream >> leftHandSideValue >> arrow))
	{
		return "";
	}

	const Symbol leftHandSide(leftHandSideValue, false);
	std::vector<std::string> tokens;
	std::string token;

	while (lineStream >> token)
	{
		tokens.push_back(token);
	}

	AddProductions(grammar, leftHandSide, tokens);
	return leftHandSideValue;
}

void GrammarLoader::AddProductions(Grammar& grammar, const Symbol& lhs, const std::vector<std::string>& tokens)
{
	std::vector<Symbol> currentRightHandSide;

	for (const std::string& token : tokens)
	{
		if (token == "|")
		{
			grammar.AddRule(lhs, currentRightHandSide);
			currentRightHandSide.clear();
			continue;
		}

		if (token == "e" || token == "ε")
		{
			continue;
		}

		currentRightHandSide.emplace_back(token, IsTerminal(token));
	}

	grammar.AddRule(lhs, currentRightHandSide);
}

bool GrammarLoader::IsTerminal(const std::string& value)
{
	if (value.empty())
	{
		return false;
	}
	const char firstCharacter = value[0];
	return !(firstCharacter >= 'A' && firstCharacter <= 'Z');
}