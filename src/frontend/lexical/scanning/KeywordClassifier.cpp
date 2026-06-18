#include "KeywordClassifier.h"

const std::unordered_map<std::string_view, TokenType> KeywordClassifier::m_keywords = {
	{ "package", TokenType::PACKAGE },
	{ "import", TokenType::IMPORT },
	{ "func", TokenType::FUNC },
	{ "type", TokenType::TYPE },
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
	{ "test", TokenType::TEST },
	{ "assert", TokenType::ASSERT },
	{ "forall", TokenType::FORALL },
	{ "defer", TokenType::DEFER },
	{ "true", TokenType::TRUE },
	{ "false", TokenType::FALSE },
	{ "nil", TokenType::NIL },
	{ "int", TokenType::IDENTIFIER },
	{ "float", TokenType::IDENTIFIER },
	{ "string", TokenType::IDENTIFIER },
	{ "bool", TokenType::IDENTIFIER }
};

TokenType KeywordClassifier::Classify(const std::string_view lexeme)
{
	const auto it = m_keywords.find(lexeme);
	return it != m_keywords.end()
		? it->second
		: TokenType::IDENTIFIER;
}
