#include "GrammarLineParser.h"

#include "SemanticTagFormatter.h"

#include <sstream>
#include <stdexcept>

namespace
{
constexpr char SEMANTIC_TAG_CHAR = '@';
constexpr char GRAMMAR_ALTERNATIVE_CHAR = '|';
constexpr auto GRAMMAR_ARROW = "->";
}

ParsedGrammarLine GrammarLineParser::Parse(const std::string& line)
{
	std::stringstream lineStream(line);
	std::string lhsValue;
	std::string arrow;

	if (!(lineStream >> lhsValue))
	{
		throw std::runtime_error("Failed to parse grammar line: " + line);
	}

	if (lhsValue.empty())
	{
		throw std::runtime_error("Grammar line contains empty lhs: " + line);
	}

	if (!(lineStream >> arrow))
	{
		throw std::runtime_error("Grammar line is missing '->': " + line);
	}

	if (arrow != GRAMMAR_ARROW)
	{
		throw std::runtime_error("Grammar line contains invalid arrow '" + arrow + "': " + line);
	}

	const Symbol lhsSymbol = ParseSymbolToken(lhsValue);
	if (lhsSymbol.IsTerminal())
	{
		throw std::runtime_error("lhs must be a non-terminal wrapped in '~': " + lhsValue);
	}

	ParsedGrammarLine parsed{ lhsSymbol, {}, {}, false };
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

		parsed.rhsTokens.push_back(token);
	}

	if (parsed.rhsTokens.empty())
	{
		throw std::runtime_error("Grammar line contains empty rhs: " + line);
	}

	for (const std::string& rhsToken : parsed.rhsTokens)
	{
		if (parsed.hasSemanticTag && rhsToken.size() == 1 && rhsToken.front() == GRAMMAR_ALTERNATIVE_CHAR)
		{
			throw std::runtime_error("Tagged grammar alternatives must be split into separate lines.");
		}
	}

	return parsed;
}

bool GrammarLineParser::IsNonTerminal(const std::string& value)
{
	return value.size() >= 2 && value.front() == '~' && value.back() == '~';
}

Symbol GrammarLineParser::ParseSymbolToken(const std::string& token)
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
