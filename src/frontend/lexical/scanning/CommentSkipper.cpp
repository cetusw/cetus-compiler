#include "CommentSkipper.h"

CommentSkipper::CommentSkipper(SourceCursor& cursor)
	: m_cursor(cursor)
{
}

bool CommentSkipper::TrySkipComment(std::optional<LexicalDiagnostic>& error) const
{
	if (m_cursor.Match('/'))
	{
		SkipSingleLineComment();
		return true;
	}

	if (m_cursor.Match('*'))
	{
		return SkipMultiLineComment(error);
	}

	return false;
}

void CommentSkipper::SkipSingleLineComment() const
{
	while (m_cursor.Peek() != '\n' && !m_cursor.IsEnd())
	{
		m_cursor.Advance();
	}
}

bool CommentSkipper::SkipMultiLineComment(std::optional<LexicalDiagnostic>& error) const
{
	const int startLine = m_cursor.GetLine();
	while (!m_cursor.IsEnd())
	{
		if (m_cursor.Peek() == '\n')
		{
			m_cursor.AdvanceLine();
		}

		if (m_cursor.Peek() == '*' && m_cursor.PeekNext() == '/')
		{
			m_cursor.Advance();
			m_cursor.Advance();
			return true;
		}

		m_cursor.Advance();
	}

	error = LexicalDiagnostic{ "Unterminated block comment", startLine };
	return false;
}
