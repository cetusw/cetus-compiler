#include "CompilerPipeline.h"

#include "src/app/cli/testing/TestReporter.h"
#include "src/app/cli/utils/utils.h"
#include "src/backend/vm/testing/TestRunner.h"
#include "src/frontend/codegen/CodegenVisitor.h"
#include "src/frontend/semantic/SemanticAnalyzer.h"
#include "src/frontend/testing/TestCoverageAnalyzer.h"
#include "src/frontend/testing/TestDiscovery.h"

#include <iostream>
#include <stdexcept>

std::unique_ptr<ProgramASTNode> CompilerPipeline::ParseFile(const Configuration& configuration)
{
	ParseResult parseResult = Utils::ParseSourceFile(configuration);

	if (!parseResult.success)
	{
		throw std::runtime_error(parseResult.message);
	}

	if (!parseResult.ast)
	{
		throw std::runtime_error(
			"Program AST was not produced for parsed input.");
	}

	return std::move(parseResult.ast);
}

TestDiscoveryResult CompilerPipeline::DiscoverTests(const ProgramASTNode& program)
{
	constexpr TestDiscovery discovery;

	const TestDiscoveryResult discoveryResult = discovery.Discover(program);

	if (!discoveryResult.diagnostics.empty())
	{
		throw std::runtime_error(discoveryResult.diagnostics.front().message);
	}

	return discoveryResult;
}

TypeCheckResult CompilerPipeline::TypeCheck(const ProgramASTNode& program)
{
	const SymbolTable symbols;
	SemanticAnalyzer checker(symbols);

	const TypeCheckResult typeResult = checker.Analyze(program);

	if (const std::optional<std::string> error = typeResult.GetErrorMessage())
	{
		throw std::runtime_error(*error);
	}

	return typeResult;
}

void CompilerPipeline::ValidateTestCoverage(const ProgramASTNode& program, const TestDiscoveryResult& discoveryResult)
{
	constexpr TestCoverageAnalyzer coverageAnalyzer;

	const std::vector<SemanticDiagnostic> diagnostics = coverageAnalyzer.Analyze(program, discoveryResult);

	if (!diagnostics.empty())
	{
		throw std::runtime_error(
			diagnostics.front().message);
	}
}

CodegenResult CompilerPipeline::Compile(const ProgramASTNode& program, const TypeCheckResult& typeResult)
{
	CodegenVisitor codegen(typeResult.symbols, typeResult);

	const CodegenResult codegenResult = codegen.Generate(program);

	if (codegenResult.error.has_value())
	{
		throw std::runtime_error(*codegenResult.error);
	}

	return codegenResult;
}

void CompilerPipeline::RunTests(const CodegenResult& codegenResult, VM& vm, const bool report)
{
	const TestRunResult result = TestRunner::Run(codegenResult.program, vm);

	if (report)
	{
		TestReporter::Print(result, std::cout);
	}

	if (result.failedCount > 0)
	{
		throw std::runtime_error("Test execution failed.");
	}
}

void CompilerPipeline::RunProgram(const CodegenResult& codegenResult, VM& vm)
{
	if (vm.InterpretProgram(codegenResult.program) != InterpretResult::OK)
	{
		throw std::runtime_error("VM execution failed.");
	}
}