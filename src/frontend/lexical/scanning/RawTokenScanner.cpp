#include "RawTokenScanner.h"

#include <cctype>

RawTokenScanner::RawTokenScanner(SourceCursor& cursor)
	: m_cursor(cursor)
	, m_literalScanner(cursor)
	, m_commentSkipper(cursor)
	, m_operatorScanner(cursor)
{
}

RawScanResult RawTokenScanner::Scan() const
{
	RawScanResult result;
	while (!m_cursor.IsEnd())
	{
		m_cursor.MarkTokenStart();
		ScanToken(result.tokens, result.error);
		if (result.error.has_value())
		{
			return result;
		}
	}

	return result;
}

void RawTokenScanner::ScanToken(
	std::vector<Token>& tokens,
	std::optional<LexicalDiagnostic>& error) const
{
	const char current = m_cursor.Advance();
	if (IsWhitespace(current))
	{
		return;
	}
	if (current == '\n')
	{
		AddNewline(tokens);
		return;
	}
	if (current == '/' && m_commentSkipper.TrySkipComment(error))
	{
		return;
	}

	if (const auto token = m_operatorScanner.TryScan(current); token.has_value())
	{
		tokens.push_back(*token);
		return;
	}
	if (const auto operatorError = m_operatorScanner.GetError(); operatorError.has_value())
	{
		error = operatorError;
		return;
	}
	if (IsDigit(current))
	{
		tokens.push_back(m_literalScanner.ScanNumber());
		return;
	}
	if (IsIdentifierStart(current))
	{
		tokens.push_back(m_literalScanner.ScanIdentifierOrKeyword());
		return;
	}
	if (current == '"' || current == '`')
	{
		const TokenScanResult result = m_literalScanner.ScanString(current);
		if (result.error.has_value())
		{
			error = result.error;
			return;
		}

		tokens.push_back(*result.token);
		return;
	}

	error = LexicalDiagnostic{
		"Unexpected character: " + std::string(1, current),
		m_cursor.GetLine()
	};
}

void RawTokenScanner::AddNewline(std::vector<Token>& tokens) const
{
	tokens.push_back(TokenFactory::Make(TokenType::NEWLINE, "\n", m_cursor.GetLine()));
	m_cursor.AdvanceLine();
}

bool RawTokenScanner::IsWhitespace(const char c)
{
	return c == ' ' || c == '\r' || c == '\t';
}

bool RawTokenScanner::IsIdentifierStart(const char c)
{
	const auto uc = static_cast<unsigned char>(c);
	return std::isalpha(uc) || c == '_';
}

bool RawTokenScanner::IsDigit(const char c)
{
	return std::isdigit(static_cast<unsigned char>(c));
}
