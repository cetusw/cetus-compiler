#pragma once

#include "src/frontend/lexical/core/Token.h"
#include "src/frontend/syntax/parser/ParseResult.h"
#include "src/frontend/syntax/table/PreparedGrammar.h"
#include <string>
#include <vector>

class SyntaxErrorBuilder
{
public:
	explicit SyntaxErrorBuilder(const PreparedGrammar& grammar);

	[[nodiscard]] ParseResult UnexpectedToken(int state, const Token& token) const;
	[[nodiscard]] static ParseResult UnexpectedEndOfStream(const Token& lastToken);
	[[nodiscard]] static ParseResult EmptyTokenStream();

private:
	[[nodiscard]] std::vector<std::string> FindExpectedTerminals(int state) const;
	[[nodiscard]] static std::string CreateMessage(
		const Token& token,
		const std::vector<std::string>& expectedTerminals);
	[[nodiscard]] static std::string DescribeToken(const Token& token);

	const PreparedGrammar& m_grammar;
};
