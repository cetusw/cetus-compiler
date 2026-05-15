#include "ParserDriver.h"

#include "src/frontend/syntax/ast/AstDumper.h"
#include "src/frontend/lexical/LexicalAnalyzer.h"
#include "src/frontend/syntax/GrammarPreparator.h"
#include "src/frontend/syntax/SyntaxAnalyzer.h"
#include "src/support/io/FileReader.h"
#include <iostream>
#include <stdexcept>

namespace
{
ParseResult ParseSourceFile(const Configuration& configuration)
{
	const std::string source = FileReader::ReadAll(configuration.inputFilePath);
	LexicalAnalyzer lexicalAnalyzer(source);
	const LexerResult lexerResult = lexicalAnalyzer.ScanTokens();
	if (lexerResult.error.has_value())
	{
		return ParseResult::Error(
			lexerResult.errorLine,
			"Lexical error at line " + std::to_string(lexerResult.errorLine) + ": " + *lexerResult.error);
	}

	GrammarPreparator preparator;
	const SyntaxAnalyzer syntaxAnalyzer(preparator.Prepare(configuration.regenerateTable));
	return syntaxAnalyzer.Analyze(lexerResult.tokens);
}
}

void ParserDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input source file path is required for parsing.");
	}

	const ParseResult result = ParseSourceFile(configuration);
	if (!result.success)
	{
		throw std::runtime_error(result.message);
	}

	std::cout << result.message << std::endl;
	if (configuration.mode == CompilerMode::PARSE_AST)
	{
		if (!result.ast)
		{
			throw std::runtime_error("AST was not produced for parsed input.");
		}

		AstDumper::Dump(*result.ast, std::cout);
	}
}
