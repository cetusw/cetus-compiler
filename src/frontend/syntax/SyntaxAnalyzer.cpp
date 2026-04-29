#include "SyntaxAnalyzer.h"

#include "grammar/GrammarLoader.h"
#include "lalr/LALRBuilder.h"
#include "parser/TableDrivenParser.h"
#include "src/frontend/lexer/Lexer.h"
#include "src/support/io/FileReader.h"
#include <stdexcept>

SyntaxAnalyzer::SyntaxAnalyzer(std::string grammarPath)
	: m_grammarPath(std::move(grammarPath))
{
}

ParseResult SyntaxAnalyzer::ParseFile(const std::string& sourceFilePath) const
{
	return ParseSource(FileReader::ReadAll(sourceFilePath));
}

ParseResult SyntaxAnalyzer::ParseSource(const std::string& source) const
{
	Lexer lexer(source);
	const std::vector<Token> tokens = lexer.ScanTokens();
	ValidateTokens(tokens);

	const Grammar grammar = GrammarLoader::LoadFromFile(m_grammarPath);
	LALRBuilder builder(grammar);
	// TODO возможно имеет смысл не пересоздавать таблицу каждый раз, а хранить её в файле и пересоздавать только с определённым флагом
	const TableDrivenParser parser(builder.Build());
	return parser.Parse(tokens);
}

void SyntaxAnalyzer::ValidateTokens(const std::vector<Token>& tokens)
{
	for (const Token& token : tokens)
	{
		if (token.type == TokenType::ERROR)
		{
			throw std::runtime_error("Lexical error at line " + std::to_string(token.line) + ": " + token.lexeme);
		}
	}
}
