#include "RunAsmExpressionDriver.h"
#include "src/frontend/codegen/AsmCodegenVisitor.h"
#include "src/frontend/syntax/SyntaxAnalyzer.h"
#include "src/frontend/syntax/semantic/TypeCheckerVisitor.h"
#include <fstream>
#include <stdexcept>
#include <utility>

void RunAsmExpressionDriver::Execute(const Configuration& configuration)
{
	const ExprPtr ast = ParseSourceFile(configuration);
	const TypeCheckResult typeResult = TypeCheckAst(*ast);
	const AsmCodegenResult codegenResult = GenerateAssembly(*ast, typeResult);
	WriteAssemblyOutput(configuration, codegenResult);
}

ExprPtr RunAsmExpressionDriver::ParseSourceFile(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input source file path is required for asm code generation.");
	}

	const SyntaxAnalyzer analyzer;
	ParseResult parseResult = analyzer.Analyze(configuration.inputFilePath);
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

TypeCheckResult RunAsmExpressionDriver::TypeCheckAst(const Expr& ast)
{
	const SymbolTable symbols;
	TypeCheckerVisitor checker(symbols);
	const TypeCheckResult typeResult = checker.Check(ast);
	if (typeResult.error.has_value())
	{
		throw std::runtime_error(*typeResult.error);
	}

	return typeResult;
}

AsmCodegenResult RunAsmExpressionDriver::GenerateAssembly(const Expr& ast, const TypeCheckResult& typeResult)
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
	std::ofstream output(outputPath);
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
