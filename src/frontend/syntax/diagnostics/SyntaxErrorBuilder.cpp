#include "SyntaxErrorBuilder.h"

#include <sstream>

SyntaxErrorBuilder::SyntaxErrorBuilder(const PreparedGrammar& grammar)
	: m_grammar(grammar)
{
}

ParseResult SyntaxErrorBuilder::UnexpectedToken(const int state, const Token& token) const
{
	std::vector<std::string> expectedTerminals = FindExpectedTerminals(state);
	std::string message = CreateMessage(token, {});
	return ParseResult::Error(
		DiagnosticStage::Syntax,
		token.line,
		std::move(message),
		std::move(expectedTerminals));
}

ParseResult SyntaxErrorBuilder::UnexpectedEndOfStream(const Token& lastToken)
{
	return ParseResult::Error(
		DiagnosticStage::Syntax,
		lastToken.line,
		"Unexpected end of token stream.");
}

ParseResult SyntaxErrorBuilder::EmptyTokenStream()
{
	return ParseResult::Error(
		DiagnosticStage::Syntax,
		1,
		"Token stream is empty.");
}

std::vector<std::string> SyntaxErrorBuilder::FindExpectedTerminals(const int state) const
{
	std::vector<std::string> expected;
	const auto stateIt = m_grammar.table.find(state);
	if (stateIt == m_grammar.table.end())
	{
		return expected;
	}

	for (const auto& [symbol, action] : stateIt->second)
	{
		if (symbol.IsTerminal() && action.type != ActionType::ERROR)
		{
			expected.push_back(symbol.GetValue());
		}
	}
	return expected;
}

std::string SyntaxErrorBuilder::CreateMessage(
	const Token& token,
	const std::vector<std::string>& expectedTerminals)
{
	std::ostringstream message;
	message << "Syntax error at line " << token.line << ": unexpected token " << DescribeToken(token);
	if (!expectedTerminals.empty())
	{
		message << ". Expected: ";
		for (std::size_t index = 0; index < expectedTerminals.size(); ++index)
		{
			if (index > 0)
			{
				message << ", ";
			}
			message << expectedTerminals[index];
		}
	}

	return message.str();
}

std::string SyntaxErrorBuilder::DescribeToken(const Token& token)
{
	if (!token.lexeme.empty())
	{
		return "'" + token.lexeme + "'";
	}

	return "at end of input";
}
