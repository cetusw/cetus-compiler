#include "RunSourceDriver.h"

#include "src/app/pipeline/CompilerPipeline.h"

#include <stdexcept>

void RunSourceDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input source file path is required for source execution.");
	}

	constexpr CompilerPipeline pipeline;

	std::unique_ptr<ProgramASTNode> program = pipeline.ParseFile(configuration);

	const TestDiscoveryResult discoveryResult = pipeline.DiscoverTests(*program);

	const TypeCheckResult typeResult = pipeline.TypeCheck(*program);

	if (configuration.requireTests)
	{
		pipeline.ValidateTestCoverage(*program, discoveryResult);
	}

	const CodegenResult codegenResult = pipeline.Compile(*program, typeResult);

	VM vm;
	vm.LoadProgram(codegenResult.program);

	if (configuration.requireTests)
	{
		pipeline.RunTests(codegenResult, vm, configuration.report);
	}

	pipeline.RunProgram(codegenResult, vm);
}