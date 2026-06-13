#include "TypeCheckDriver.h"

#include "../../../frontend/semantic/rules/TypeRules.h"
#include "src/frontend/lexical/LexicalAnalyzer.h"
#include "src/frontend/semantic/SemanticAnalyzer.h"
#include "src/frontend/testing/TestCoverageAnalyzer.h"
#include "src/frontend/testing/TestDiscovery.h"
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

const ProgramASTNode& RequireProgramAst(const ASTNode& ast)
{
	const auto* program = dynamic_cast<const ProgramASTNode*>(&ast);
	if (!program)
	{
		throw std::runtime_error("Top-level AST is not ProgramASTNode.");
	}
	return *program;
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

	const ProgramASTNode& program = RequireProgramAst(*parseResult.ast);
	const TestDiscovery discovery;
	const TestDiscoveryResult discoveryResult = discovery.Discover(program);
	if (!discoveryResult.diagnostics.empty())
	{
		throw std::runtime_error(discoveryResult.diagnostics.front().message);
	}

	SemanticAnalyzer checker{ SymbolTable() };
	const TypeCheckResult typeResult = checker.Analyze(program);
	if (const std::optional<std::string> error = typeResult.GetErrorMessage())
	{
		throw std::runtime_error(*error);
	}
	if (configuration.requireTests)
	{
		const TestCoverageAnalyzer coverageAnalyzer;
		const std::vector<SemanticDiagnostic> coverageDiagnostics = coverageAnalyzer.Analyze(program, discoveryResult);
		if (!coverageDiagnostics.empty())
		{
			throw std::runtime_error(coverageDiagnostics.front().message);
		}
	}

	std::cout << "Type: " << TypeRules::ToString(typeResult.type) << std::endl;
}
