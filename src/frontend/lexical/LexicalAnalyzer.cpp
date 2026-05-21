#include "LexicalAnalyzer.h"
#include <cctype>
#include <utility>

const std::unordered_map<std::string_view, TokenType> LexicalAnalyzer::m_keywords = {
	{ "package", TokenType::PACKAGE },
	{ "import", TokenType::IMPORT },
	{ "func", TokenType::FUNC },
	{ "struct", TokenType::STRUCT },
	{ "var", TokenType::VAR },
	{ "const", TokenType::CONST },
	{ "if", TokenType::IF },
	{ "else", TokenType::ELSE },
	{ "for", TokenType::FOR },
	{ "range", TokenType::RANGE },
	{ "break", TokenType::BREAK },
	{ "continue", TokenType::CONTINUE },
	{ "switch", TokenType::SWITCH },
	{ "case", TokenType::CASE },
	{ "default", TokenType::DEFAULT },
	{ "match", TokenType::MATCH },
	{ "return", TokenType::RETURN },
	{ "defer", TokenType::DEFER },
	{ "printf", TokenType::PRINTF },
	{ "true", TokenType::TRUE },
	{ "false", TokenType::FALSE },
	{ "nil", TokenType::NIL },
	{ "int", TokenType::IDENTIFIER },
	{ "float", TokenType::IDENTIFIER },
	{ "string", TokenType::IDENTIFIER },
	{ "bool", TokenType::IDENTIFIER }
};

LexicalAnalyzer::LexicalAnalyzer(std::string source)
	: m_source(std::move(source))
{
}

LexerResult LexicalAnalyzer::ScanTokens()
{
	while (!IsEnd())
	{
		m_start = m_current;
		ScanToken();
		if (m_error.has_value())
		{
			return { std::move(m_tokens), std::move(m_error), m_line };
		}
	}

	AddToken(TokenType::EOF_TOKEN, "", GetEofLine());
	return { std::move(m_tokens), std::nullopt, GetEofLine() };
}

void LexicalAnalyzer::ScanToken()
{
	const char c = Advance();

	if (HandleWhitespace(c) || HandlePunctuation(c) || HandleOperator(c))
	{
		return;
	}

	const auto uc = static_cast<unsigned char>(c);
	if (std::isdigit(uc))
	{
		Number();
	}
	else if (std::isalpha(uc) || c == '_')
	{
		Identifier();
	}
	else
	{
		ReportError("Unexpected character: " + std::string(1, c));
	}
}

void LexicalAnalyzer::Number()
{
	PeekDigits();
	if (Peek() == '.' && std::isdigit(PeekNext()))
	{
		Advance();
		PeekDigits();
		AddToken(TokenType::FLOAT_LIT);
	}
	else
	{
		AddToken(TokenType::INT_LIT);
	}
}

void LexicalAnalyzer::PeekDigits()
{
	while (std::isdigit(static_cast<unsigned char>(Peek())))
	{
		Advance();
	}
}

void LexicalAnalyzer::String(const char quoteType)
{
	const int startLine = m_line;
	while (Peek() != quoteType && !IsEnd())
	{
		PeekEndOfLine();
		if (Peek() == '\\' && quoteType == '"')
		{
			Advance();
		}
		Advance();
	}
	if (IsEnd())
	{
		ReportError("Unterminated string", startLine);
		return;
	}
	Advance();
	const std::string value = m_source.substr(m_start + 1, m_current - m_start - 2);
	AddToken(TokenType::STRING, value);
}

void LexicalAnalyzer::PeekEndOfLine()
{
	if (Peek() == '\n')
	{
		m_line++;
	}
}

void LexicalAnalyzer::Identifier()
{
	while (std::isalnum(static_cast<unsigned char>(Peek())) || Peek() == '_')
	{
		Advance();
	}
	AddToken(MatchKeyword());
}

TokenType LexicalAnalyzer::MatchKeyword() const
{
	const std::string_view text(m_source.data() + m_start, m_current - m_start);
	const auto it = m_keywords.find(text);
	return it != m_keywords.end()
		? it->second
		: TokenType::IDENTIFIER;
}

bool LexicalAnalyzer::IsEnd() const
{
	return m_current >= static_cast<int>(m_source.length());
}

char LexicalAnalyzer::Advance()
{
	return m_source[m_current++];
}

char LexicalAnalyzer::Peek() const
{
	return IsEnd() ? '\0' : m_source[m_current];
}

char LexicalAnalyzer::PeekNext() const
{
	return m_current + 1 >= static_cast<int>(m_source.length()) ? '\0' : m_source[m_current + 1];
}

bool LexicalAnalyzer::Match(const char expected)
{
	if (IsEnd() || m_source[m_current] != expected)
	{
		return false;
	}
	m_current++;
	return true;
}

void LexicalAnalyzer::SkipSingleLineComment()
{
	while (Peek() != '\n' && !IsEnd())
	{
		Advance();
	}
}

void LexicalAnalyzer::SkipMultiLineComment()
{
	const int startLine = m_line;
	while (!IsEnd())
	{
		PeekEndOfLine();
		if (Peek() == '*' && PeekNext() == '/')
		{
			Advance();
			Advance();
			return;
		}
		Advance();
	}

	ReportError("Unterminated block comment", startLine);
}

bool LexicalAnalyzer::HandleWhitespace(const char c)
{
	switch (c)
	{
	case ' ':
	case '\r':
	case '\t':
		return true;
	case '\n':
		m_line++;
		return true;
	case '/':
		return HandleComment();
	default:
		return false;
	}
}

bool LexicalAnalyzer::HandlePunctuation(const char c)
{
	switch (c)
	{
	case '(':
		AddToken(TokenType::LPAREN, "(");
		return true;
	case ')':
		AddToken(TokenType::RPAREN, ")");
		return true;
	case '{':
		AddToken(TokenType::LBRACE, "{");
		return true;
	case '}':
		AddToken(TokenType::RBRACE, "}");
		return true;
	case '[':
		AddToken(TokenType::LBRACKET, "[");
		return true;
	case ']':
		AddToken(TokenType::RBRACKET, "]");
		return true;
	case ',':
		AddToken(TokenType::COMMA, ",");
		return true;
	case ';':
		AddToken(TokenType::SEMICOLON, ";");
		return true;
	case '*':
		AddToken(TokenType::STAR, "*");
		return true;
	case '%':
		AddToken(TokenType::PERCENT, "%");
		return true;
	case '^':
		AddToken(TokenType::CARET, "^");
		return true;
	case '"':
		String('"');
		return true;
	case '`':
		String('`');
		return true;
	default:
		return false;
	}
}

bool LexicalAnalyzer::HandleLess()
{
	if (Match('<'))
	{
		AddToken(TokenType::LEFT_SHIFT);
	}
	else if (Match('='))
	{
		AddToken(TokenType::LESS_EQUAL);
	}
	else
	{
		AddToken(TokenType::LESS);
	}
	return true;
}

bool LexicalAnalyzer::HandleGreater()
{
	if (Match('>'))
	{
		AddToken(TokenType::RIGHT_SHIFT);
	}
	else if (Match('='))
	{
		AddToken(TokenType::GREATER_EQUAL);
	}
	else
	{
		AddToken(TokenType::GREATER);
	}
	return true;
}

bool LexicalAnalyzer::HandleDot()
{
	if (Match('.'))
	{
		if (Match('.'))
		{
			AddToken(TokenType::ELLIPSIS);
		}
		else
		{
			ReportError("Unexpected '..', did you mean '...'?");
		}
	}
	else
	{
		AddToken(TokenType::DOT);
	}
	return true;
}

bool LexicalAnalyzer::HandleAmpersand()
{
	if (Match('&'))
	{
		AddToken(TokenType::AND_AND);
	}
	else if (Match('^'))
	{
		AddToken(TokenType::AND_NOT);
	}
	else
	{
		AddToken(TokenType::BIT_AND);
	}
	return true;
}

bool LexicalAnalyzer::HandleQuestion()
{
	if (Match('.'))
	{
		AddToken(TokenType::QUESTION_DOT);
	}
	else if (Match('?'))
	{
		AddToken(TokenType::QUESTION_QUESTION);
	}
	else
	{
		ReportError(R"(Unexpected '?', did you mean '?.' or '??'?)");
	}
	return true;
}

bool LexicalAnalyzer::HandleComment()
{
	if (Match('/'))
	{
		SkipSingleLineComment();
		return true;
	}
	if (Match('*'))
	{
		SkipMultiLineComment();
		return true;
	}
	return false;
}

bool LexicalAnalyzer::HandleOperator(const char c)
{
	switch (c)
	{
	case '/':
		AddToken(TokenType::SLASH, "/");
		return true;
	case '!':
		return MatchToken('=', TokenType::BANG_EQUAL, TokenType::BANG);
	case '=':
		return MatchToken('=', TokenType::EQUAL_EQUAL, TokenType::EQUAL);
	case ':':
		return MatchToken('=', TokenType::COLON_EQUAL, TokenType::COLON);
	case '<':
		return HandleLess();
	case '>':
		return HandleGreater();
	case '.':
		return HandleDot();
	case '&':
		return HandleAmpersand();
	case '?':
		return HandleQuestion();
	case '+':
		return MatchToken('+', TokenType::PLUS_PLUS, TokenType::PLUS);
	case '-':
		return MatchToken('-', TokenType::MINUS_MINUS, TokenType::MINUS);
	case '|':
		return MatchToken('|', TokenType::OR_OR, TokenType::BIT_OR);
	default:
		return false;
	}
}

void LexicalAnalyzer::AddToken(const TokenType type, std::string lexeme)
{
	AddToken(type, std::move(lexeme), m_line);
}

void LexicalAnalyzer::AddToken(const TokenType type, std::string lexeme, const int line)
{
	m_tokens.push_back({ type, std::move(lexeme), line });
}

int LexicalAnalyzer::GetEofLine() const
{
	if (m_tokens.empty())
	{
		return 1;
	}

	return m_tokens.back().line;
}

void LexicalAnalyzer::AddToken(const TokenType type)
{
	AddToken(type, m_source.substr(m_start, m_current - m_start), m_line);
}

void LexicalAnalyzer::ReportError(std::string message)
{
	ReportError(std::move(message), m_line);
}

void LexicalAnalyzer::ReportError(std::string message, const int line)
{
	if (!m_error.has_value())
	{
		m_error = std::move(message);
		m_line = line;
	}
}

bool LexicalAnalyzer::MatchToken(const char expected, const TokenType ifMatch, const TokenType ifNoMatch)
{
	if (Match(expected))
	{
		AddToken(ifMatch);
	}
	else
	{
		AddToken(ifNoMatch);
	}

	return true;
}
