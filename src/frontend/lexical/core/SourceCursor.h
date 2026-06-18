#pragma once

#include <string>
#include <string_view>

class SourceCursor
{
public:
	explicit SourceCursor(const std::string& source);

	[[nodiscard]] bool IsEnd() const;
	[[nodiscard]] char Peek() const;
	[[nodiscard]] char PeekNext() const;
	[[nodiscard]] char Previous() const;

	char Advance();
	bool Match(char expected);

	void MarkTokenStart();
	[[nodiscard]] std::string CurrentLexeme() const;
	[[nodiscard]] std::string_view CurrentLexemeView() const;

	[[nodiscard]] int GetLine() const;
	void AdvanceLine();

private:
	const std::string& m_source;
	int m_start = 0;
	int m_current = 0;
	int m_line = 1;
};
