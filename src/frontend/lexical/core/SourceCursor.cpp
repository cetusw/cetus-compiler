#include "SourceCursor.h"

SourceCursor::SourceCursor(const std::string& source)
	: m_source(source)
{
}

bool SourceCursor::IsEnd() const
{
	return m_current >= static_cast<int>(m_source.length());
}

char SourceCursor::Peek() const
{
	return IsEnd()
		? '\0'
		: m_source[m_current];
}

char SourceCursor::PeekNext() const
{
	return m_current + 1 >= static_cast<int>(m_source.length())
		? '\0'
		: m_source[m_current + 1];
}

char SourceCursor::Previous() const
{
	return m_current == 0
		? '\0'
		: m_source[m_current - 1];
}

char SourceCursor::Advance()
{
	return m_source[m_current++];
}

bool SourceCursor::Match(const char expected)
{
	if (IsEnd() || m_source[m_current] != expected)
	{
		return false;
	}

	++m_current;
	return true;
}

void SourceCursor::MarkTokenStart()
{
	m_start = m_current;
}

std::string SourceCursor::CurrentLexeme() const
{
	return std::string(CurrentLexemeView());
}

std::string_view SourceCursor::CurrentLexemeView() const
{
	return { m_source.data() + m_start, static_cast<std::size_t>(m_current - m_start) };
}

int SourceCursor::GetLine() const
{
	return m_line;
}

void SourceCursor::AdvanceLine()
{
	++m_line;
}
