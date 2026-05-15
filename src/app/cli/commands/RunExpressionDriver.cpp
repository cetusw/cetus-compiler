#include "RunExpressionDriver.h"

#include "../../../frontend/semantic/rules/TypeRules.h"
#include "PredefinedSymbols.h"
#include "src/backend/vm/vm.h"
#include "src/frontend/codegen/CodegenVisitor.h"
#include "src/frontend/lexical/LexicalAnalyzer.h"
#include "src/frontend/semantic/SemanticAnalyzer.h"
#include "src/frontend/syntax/GrammarPreparator.h"
#include "src/frontend/syntax/SyntaxAnalyzer.h"
#include "src/support/io/FileReader.h"
#include <cstdio>
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

void RunExpressionDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input source file path is required for expression execution.");
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

	const SymbolTable symbols = PredefinedSymbols::CreateSymbolTable();
	SemanticAnalyzer checker(symbols);
	const TypeCheckResult typeResult = checker.Analyze(*parseResult.ast);
	if (const std::optional<std::string> error = typeResult.GetErrorMessage())
	{
		throw std::runtime_error(*error);
	}

	CodegenVisitor codegen(symbols, typeResult);
	const CodegenResult codegenResult = codegen.Generate(*parseResult.ast);
	if (codegenResult.error.has_value())
	{
		throw std::runtime_error(*codegenResult.error);
	}

	VM vm;
	PredefinedSymbols::LoadRuntimeGlobals(vm);
	if (vm.Interpret(codegenResult.function) != InterpretResult::OK)
	{
		throw std::runtime_error("VM execution failed.");
	}

	// TODO Убрать, как только будет весь язык
	std::printf("Type: %s\nResult: ", TypeRules::ToString(typeResult.type));
	vm.GetLastResult().Print();
	std::printf("\n");
}
