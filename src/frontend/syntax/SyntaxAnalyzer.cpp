#include "SyntaxAnalyzer.h"
#include "grammar/GrammarLoader.h"
#include "lalr/LALRBuilder.h"
#include "parser/TableDrivenParser.h"
#include "src/frontend/lexer/Lexer.h"
#include "src/support/io/FileReader.h"

SyntaxAnalyzer::SyntaxAnalyzer(std::string grammarPath)
	: m_grammarPath(std::move(grammarPath))
{
}

ParseResult SyntaxAnalyzer::Analyze(const std::string& sourceFilePath) const
{
	const std::string source = FileReader::ReadAll(sourceFilePath);
	return ParseSource(source);
}

ParseResult SyntaxAnalyzer::ParseSource(const std::string& source) const
{
	Lexer lexer(source);
	LexerResult lexerResult = lexer.ScanTokens();
	if (lexerResult.error.has_value())
	{
		return ParseResult::Error(
			lexerResult.errorLine,
			"Lexical error at line " + std::to_string(lexerResult.errorLine) + ": " + *lexerResult.error);
	}

	const Grammar grammar = GrammarLoader::LoadFromFile(m_grammarPath);
	LALRBuilder builder(grammar);
	// TODO возможно имеет смысл не пересоздавать таблицу каждый раз, а хранить её в файле и пересоздавать только с определённым флагом
	const TableDrivenParser parser(builder.Build());
	return parser.Parse(lexerResult.tokens);
}
