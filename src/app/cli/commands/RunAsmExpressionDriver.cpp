#include "RunAsmExpressionDriver.h"
#include "../../../frontend/codegen/asm/AsmCodegenVisitor.h"
#include "src/frontend/semantic/SemanticAnalyzer.h"
#include "src/frontend/syntax/GrammarPreparator.h"
#include "src/frontend/syntax/SyntaxAnalyzer.h"
#include "src/support/io/FileReader.h"
#include <stdexcept>
#include <utility>

void RunAsmExpressionDriver::Execute(const Configuration& configuration)
{
	const std::string source = ReadSource(configuration);
	const std::vector<Token> tokens = Tokenize(source);
	const ASTNodePtr ast = ParseTokens(tokens, configuration.regenerateTable);
	const TypeCheckResult typeResult = TypeCheckAst(*ast);
	const AsmCodegenResult asmResult = GenerateAssembly(*ast, typeResult);
	WriteAssemblyOutput(configuration, asmResult);
}

std::string RunAsmExpressionDriver::ReadSource(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input source file path is required for asm code generation.");
	}
	return FileReader::ReadAll(configuration.inputFilePath);
}

std::vector<Token> RunAsmExpressionDriver::Tokenize(const std::string& source)
{
	LexicalAnalyzer lexicalAnalyzer(source);
	LexerResult lexerResult = lexicalAnalyzer.ScanTokens();
	if (lexerResult.error.has_value())
	{
		throw std::runtime_error(
		"Lexical error at line " + std::to_string(lexerResult.errorLine) + ": " + *lexerResult.error);
	}
	return lexerResult.tokens;
}

ASTNodePtr RunAsmExpressionDriver::ParseTokens(const std::vector<Token>& tokens, const bool regenerateTable)
{
	GrammarPreparator preparator;
	const PreparedGrammar preparedGrammar = preparator.Prepare(regenerateTable);

	const SyntaxAnalyzer syntaxAnalyzer(preparedGrammar);
	ParseResult parseResult = syntaxAnalyzer.Analyze(tokens);
	if (!parseResult.success)
	{
		throw std::runtime_error(parseResult.message);
	}
	if (!parseResult.ast)
	{
		throw std::runtime_error("AST was not produced for parsed input.");
	}

	return std::move(parseResult.ast);
}

TypeCheckResult RunAsmExpressionDriver::TypeCheckAst(const ASTNode& ast)
{
	const SymbolTable symbols;
	SemanticAnalyzer semanticAnalyzer(symbols);
	const TypeCheckResult typeResult = semanticAnalyzer.Analyze(ast);
	if (const std::optional<std::string> error = typeResult.GetErrorMessage())
	{
		throw std::runtime_error(*error);
	}

	return typeResult;
}

AsmCodegenResult RunAsmExpressionDriver::GenerateAssembly(const ASTNode& ast, const TypeCheckResult& typeResult)
{
	AsmCodegenVisitor codegen(typeResult);
	const AsmCodegenResult codegenResult = codegen.Generate(ast);
	if (codegenResult.error.has_value())
	{
		throw std::runtime_error(*codegenResult.error);
	}

	return codegenResult;
}

void RunAsmExpressionDriver::WriteAssemblyOutput(
	const Configuration& configuration,
	const AsmCodegenResult& codegenResult)
{
	const std::string outputPath = configuration.outputFilePath.empty() ? "out.asm" : configuration.outputFilePath;
	FileReader::WriteAll(outputPath, codegenResult.text);
}
