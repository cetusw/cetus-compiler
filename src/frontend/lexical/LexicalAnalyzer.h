#pragma once
#include "Token.h"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct LexerResult
{
	std::vector<Token> tokens;
	std::optional<std::string> error;
	int errorLine = 1;
};

class LexicalAnalyzer
{
public:
	explicit LexicalAnalyzer(std::string source);
	LexerResult ScanTokens();

private:
	std::string m_source;
	std::vector<Token> m_tokens;
	std::optional<std::string> m_error;
	int m_start = 0;
	int m_current = 0;
	int m_line = 1;

	static const std::unordered_map<std::string_view, TokenType> m_keywords;

	void ScanToken();

	bool HandleLess();
	bool HandleGreater();
	bool HandleDot();
	bool HandleAmpersand();
	bool HandleQuestion();
	bool HandleComment();

	bool HandleWhitespace(char c);
	bool HandlePunctuation(char c);
	bool HandleOperator(char c);

	[[nodiscard]] int GetEofLine() const;
	void AddToken(TokenType type, std::string lexeme);
	void AddToken(TokenType type, std::string lexeme, int line);
	void AddToken(TokenType type);
	void ReportError(std::string message, int line);
	void ReportError(std::string message);

	bool MatchToken(char expected, TokenType ifMatch, TokenType ifNoMatch);

	[[nodiscard]] TokenType MatchKeyword() const;

	void Identifier();
	void Number();
	void String(char quoteType);

	void PeekDigits();
	void PeekEndOfLine();

	void SkipSingleLineComment();
	void SkipMultiLineComment();

	[[nodiscard]] bool IsEnd() const;
	char Advance();
	[[nodiscard]] char Peek() const;
	[[nodiscard]] char PeekNext() const;
	bool Match(char expected);
};
