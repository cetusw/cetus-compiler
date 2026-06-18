#include "LiteralScanner.h"

#include "KeywordClassifier.h"

#include <cctype>

LiteralScanner::LiteralScanner(SourceCursor& cursor)
	: m_cursor(cursor)
{
}

Token LiteralScanner::ScanIdentifierOrKeyword() const
{
	while (IsIdentifierPart(m_cursor.Peek()))
	{
		m_cursor.Advance();
	}

	const std::string lexeme = m_cursor.CurrentLexeme();
	const TokenType type = KeywordClassifier::Classify(m_cursor.CurrentLexemeView());
	return TokenFactory::Make(type, lexeme, m_cursor.GetLine());
}

Token LiteralScanner::ScanNumber() const
{
	while (IsDigit(m_cursor.Peek()))
	{
		m_cursor.Advance();
	}

	if (m_cursor.Peek() == '.' && IsDigit(m_cursor.PeekNext()))
	{
		m_cursor.Advance();
		while (IsDigit(m_cursor.Peek()))
		{
			m_cursor.Advance();
		}

		return TokenFactory::Make(
			TokenType::FLOAT_LIT,
			m_cursor.CurrentLexeme(),
			m_cursor.GetLine());
	}

	return TokenFactory::Make(
		TokenType::INT_LIT,
		m_cursor.CurrentLexeme(),
		m_cursor.GetLine());
}

TokenScanResult LiteralScanner::ScanString(const char quoteType) const
{
	const int startLine = m_cursor.GetLine();
	while (m_cursor.Peek() != quoteType && !m_cursor.IsEnd())
	{
		if (m_cursor.Peek() == '\n')
		{
			m_cursor.AdvanceLine();
		}
		if (m_cursor.Peek() == '\\' && quoteType == '"')
		{
			m_cursor.Advance();
		}
		m_cursor.Advance();
	}

	if (m_cursor.IsEnd())
	{
		return {
			std::nullopt,
			LexicalDiagnostic{ "Unterminated string", startLine }
		};
	}

	m_cursor.Advance();
	const std::string_view lexeme = m_cursor.CurrentLexemeView();
	return {
		TokenFactory::Make(
			TokenType::STRING,
			std::string(lexeme.substr(1, lexeme.size() - 2)),
			startLine),
		std::nullopt
	};
}

bool LiteralScanner::IsIdentifierPart(const char c)
{
	const auto uc = static_cast<unsigned char>(c);
	return std::isalnum(uc) || c == '_';
}

bool LiteralScanner::IsDigit(const char c)
{
	return std::isdigit(static_cast<unsigned char>(c));
}
