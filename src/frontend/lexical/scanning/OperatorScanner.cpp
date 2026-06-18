#include "OperatorScanner.h"

OperatorScanner::OperatorScanner(SourceCursor& cursor)
	: m_cursor(cursor)
{
}

std::optional<Token> OperatorScanner::TryScan(const char current) const
{
	m_error.reset();

	switch (current)
	{
	case '(':
		return TokenFactory::Make(TokenType::LPAREN, "(", m_cursor.GetLine());
	case ')':
		return TokenFactory::Make(TokenType::RPAREN, ")", m_cursor.GetLine());
	case '{':
		return TokenFactory::Make(TokenType::LBRACE, "{", m_cursor.GetLine());
	case '}':
		return TokenFactory::Make(TokenType::RBRACE, "}", m_cursor.GetLine());
	case '[':
		return TokenFactory::Make(TokenType::LBRACKET, "[", m_cursor.GetLine());
	case ']':
		return TokenFactory::Make(TokenType::RBRACKET, "]", m_cursor.GetLine());
	case ',':
		return TokenFactory::Make(TokenType::COMMA, ",", m_cursor.GetLine());
	case ';':
		return TokenFactory::Make(TokenType::SEMICOLON, ";", m_cursor.GetLine());
	case '*':
		return TokenFactory::Make(TokenType::STAR, "*", m_cursor.GetLine());
	case '%':
		return TokenFactory::Make(TokenType::PERCENT, "%", m_cursor.GetLine());
	case '^':
		return TokenFactory::Make(TokenType::CARET, "^", m_cursor.GetLine());
	case '/':
		return TokenFactory::Make(TokenType::SLASH, "/", m_cursor.GetLine());
	case '!':
		return MatchToken('=', TokenType::BANG_EQUAL, TokenType::BANG);
	case '=':
		return MatchToken('=', TokenType::EQUAL_EQUAL, TokenType::EQUAL);
	case ':':
		return MatchToken('=', TokenType::COLON_EQUAL, TokenType::COLON);
	case '<':
		return ScanLess();
	case '>':
		return ScanGreater();
	case '.':
		return ScanDot();
	case '&':
		return ScanAmpersand();
	case '?':
		return ScanQuestion();
	case '+':
		return MatchToken('+', TokenType::PLUS_PLUS, TokenType::PLUS);
	case '-':
		return MatchToken('-', TokenType::MINUS_MINUS, TokenType::MINUS);
	case '|':
		return MatchToken('|', TokenType::OR_OR, TokenType::BIT_OR);
	default:
		return std::nullopt;
	}
}

std::optional<LexicalDiagnostic> OperatorScanner::GetError() const
{
	return m_error;
}

Token OperatorScanner::MatchToken(const char expected, const TokenType ifMatch, const TokenType ifNoMatch) const
{
	const TokenType type = m_cursor.Match(expected) ? ifMatch : ifNoMatch;
	return TokenFactory::Make(type, m_cursor.CurrentLexeme(), m_cursor.GetLine());
}

Token OperatorScanner::ScanLess() const
{
	if (m_cursor.Match('<'))
	{
		return TokenFactory::Make(TokenType::LEFT_SHIFT, m_cursor.CurrentLexeme(), m_cursor.GetLine());
	}
	if (m_cursor.Match('='))
	{
		return TokenFactory::Make(TokenType::LESS_EQUAL, m_cursor.CurrentLexeme(), m_cursor.GetLine());
	}

	return TokenFactory::Make(TokenType::LESS, m_cursor.CurrentLexeme(), m_cursor.GetLine());
}

Token OperatorScanner::ScanGreater() const
{
	if (m_cursor.Match('>'))
	{
		return TokenFactory::Make(TokenType::RIGHT_SHIFT, m_cursor.CurrentLexeme(), m_cursor.GetLine());
	}
	if (m_cursor.Match('='))
	{
		return TokenFactory::Make(TokenType::GREATER_EQUAL, m_cursor.CurrentLexeme(), m_cursor.GetLine());
	}

	return TokenFactory::Make(TokenType::GREATER, m_cursor.CurrentLexeme(), m_cursor.GetLine());
}

std::optional<Token> OperatorScanner::ScanDot() const
{
	if (!m_cursor.Match('.'))
	{
		return TokenFactory::Make(TokenType::DOT, ".", m_cursor.GetLine());
	}

	if (m_cursor.Match('.'))
	{
		return TokenFactory::Make(TokenType::ELLIPSIS, m_cursor.CurrentLexeme(), m_cursor.GetLine());
	}

	m_error = LexicalDiagnostic{
		"Unexpected '..', did you mean '...'?",
		m_cursor.GetLine()
	};
	return std::nullopt;
}

Token OperatorScanner::ScanAmpersand() const
{
	if (m_cursor.Match('&'))
	{
		return TokenFactory::Make(TokenType::AND_AND, m_cursor.CurrentLexeme(), m_cursor.GetLine());
	}
	if (m_cursor.Match('^'))
	{
		return TokenFactory::Make(TokenType::AND_NOT, m_cursor.CurrentLexeme(), m_cursor.GetLine());
	}

	return TokenFactory::Make(TokenType::BIT_AND, m_cursor.CurrentLexeme(), m_cursor.GetLine());
}

std::optional<Token> OperatorScanner::ScanQuestion() const
{
	if (m_cursor.Match('.'))
	{
		return TokenFactory::Make(TokenType::QUESTION_DOT, m_cursor.CurrentLexeme(), m_cursor.GetLine());
	}
	if (m_cursor.Match('?'))
	{
		return TokenFactory::Make(TokenType::QUESTION_QUESTION, m_cursor.CurrentLexeme(), m_cursor.GetLine());
	}

	m_error = LexicalDiagnostic{
		R"(Unexpected '?', did you mean '?.' or '??'?)",
		m_cursor.GetLine()
	};
	return std::nullopt;
}
