#include "RunAsmExpressionDriver.h"

#include "src/frontend/codegen/AsmCodegenVisitor.h"
#include "src/frontend/syntax/SyntaxAnalyzer.h"
#include "src/frontend/syntax/semantic/TypeCheckerVisitor.h"
#include <fstream>
#include <stdexcept>

void RunAsmExpressionDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input source file path is required for asm code generation.");
	}

	const SyntaxAnalyzer analyzer;
	const ParseResult parseResult = analyzer.ParseFile(configuration.inputFilePath);
	if (!parseResult.success)
	{
		throw std::runtime_error(parseResult.message);
	}
	if (!parseResult.ast)
	{
		throw std::runtime_error("AST was not produced for parsed input.");
	}

	const SymbolTable symbols;
	TypeCheckerVisitor checker(symbols);
	const TypeCheckResult typeResult = checker.Check(*parseResult.ast);
	if (typeResult.error.has_value())
	{
		throw std::runtime_error(*typeResult.error);
	}

	AsmCodegenVisitor codegen(typeResult);
	const AsmCodegenResult codegenResult = codegen.Generate(*parseResult.ast);
	if (codegenResult.error.has_value())
	{
		throw std::runtime_error(*codegenResult.error);
	}

	const std::string outputPath = configuration.outputFilePath.empty() ? "out.s" : configuration.outputFilePath;
	std::ofstream output(outputPath, std::ios::binary | std::ios::trunc);
	if (!output)
	{
		throw std::runtime_error("Failed to open output file for asm code generation: " + outputPath);
	}

	output << codegenResult.text;
	if (!output)
	{
		throw std::runtime_error("Failed to write asm output file: " + outputPath);
	}
}
