#include "TypeCheckDriver.h"

#include "../../../frontend/semantic/rules/TypeRules.h"
#include "PredefinedSymbols.h"
#include "src/frontend/lexical/LexicalAnalyzer.h"
#include "src/frontend/semantic/SemanticAnalyzer.h"
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

void TypeCheckDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input source file path is required for type checking.");
	}

	const ParseResult parseResult = ParseSourceFile(configuration);
	if (!parseResult.success)
	{
		throw std::runtime_error(parseResult.message);
	}
	if (!parseResult.ast)
	{
		throw std::runtime_error("AST was not produced for parsed input.");
	}

	SemanticAnalyzer checker(PredefinedSymbols::CreateSymbolTable());
	const TypeCheckResult typeResult = checker.Analyze(*parseResult.ast);
	if (const std::optional<std::string> error = typeResult.GetErrorMessage())
	{
		throw std::runtime_error(*error);
	}

	std::cout << "Type: " << TypeRules::ToString(typeResult.type) << std::endl;
}
