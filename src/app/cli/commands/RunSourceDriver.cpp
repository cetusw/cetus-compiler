#include "RunSourceDriver.h"

#include "src/app/cli/testing/TestReporter.h"
#include "src/app/cli/utils/utils.h"
#include "src/backend/vm/testing/TestRunner.h"
#include "src/backend/vm/vm.h"
#include "src/frontend/codegen/CodegenVisitor.h"
#include "src/frontend/semantic/SemanticAnalyzer.h"
#include "src/frontend/testing/TestCoverageAnalyzer.h"
#include "src/frontend/testing/TestDiscovery.h"

#include <iostream>
#include <stdexcept>

void RunSourceDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input source file path is required for source execution.");
	}

	const ParseResult parseResult = Utils::ParseSourceFile(configuration);
	if (!parseResult.success)
	{
		throw std::runtime_error(parseResult.message);
	}
	if (!parseResult.ast)
	{
		throw std::runtime_error("Program AST was not produced for parsed input.");
	}

	const ProgramASTNode& program = *parseResult.ast;

	const TestDiscovery discovery;
	const TestDiscoveryResult discoveryResult = discovery.Discover(program);
	if (!discoveryResult.diagnostics.empty())
	{
		throw std::runtime_error(discoveryResult.diagnostics.front().message);
	}

	const SymbolTable symbols;
	SemanticAnalyzer checker(symbols);
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

	CodegenVisitor codegen(typeResult.symbols, typeResult);
	const CodegenResult codegenResult = codegen.Generate(program);
	if (codegenResult.error.has_value())
	{
		throw std::runtime_error(*codegenResult.error);
	}

	VM vm;
	vm.LoadProgram(codegenResult.program);
	if (configuration.requireTests)
	{
		const TestRunResult result = TestRunner::Run(codegenResult.program, vm);
		if (configuration.report)
		{
			TestReporter::Print(result, std::cout);
		}
		if (result.failedCount > 0)
		{
			throw std::runtime_error("Test execution failed.");
		}
	}
	if (vm.InterpretProgram(codegenResult.program) != InterpretResult::OK)
	{
		throw std::runtime_error("VM execution failed.");
	}
}
