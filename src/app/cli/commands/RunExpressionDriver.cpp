#include "RunExpressionDriver.h"

#include "PredefinedSymbols.h"
#include "src/backend/vm/vm.h"
#include "src/frontend/codegen/CodegenVisitor.h"
#include "src/frontend/syntax/SyntaxAnalyzer.h"
#include "src/frontend/syntax/semantic/TypeCheckerVisitor.h"
#include "src/frontend/syntax/semantic/TypeRules.h"
#include <cstdio>
#include <stdexcept>

void RunExpressionDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input source file path is required for expression execution.");
	}

	const SyntaxAnalyzer analyzer;
	const ParseResult parseResult = analyzer.Analyze(configuration.inputFilePath);
	if (!parseResult.success)
	{
		throw std::runtime_error(parseResult.message);
	}
	if (!parseResult.ast)
	{
		throw std::runtime_error("AST was not produced for parsed input.");
	}

	const SymbolTable symbols = PredefinedSymbols::CreateSymbolTable();
	TypeCheckerVisitor checker(symbols);
	const TypeCheckResult typeResult = checker.Check(*parseResult.ast);
	if (typeResult.error.has_value())
	{
		throw std::runtime_error(*typeResult.error);
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
