#include "RunSourceDriver.h"

#include "src/app/diagnostics/CompilerError.h"
#include "src/app/diagnostics/DiagnosticStage.h"
#include "src/app/pipeline/CompilerPipeline.h"

void RunSourceDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw CompilerError(
			DiagnosticStage::Internal,
			"Input source file path is required for source execution.");
	}
	
	std::unique_ptr<ProgramASTNode> program = CompilerPipeline::ParseFile(configuration);

	const TestDiscoveryResult discoveryResult = CompilerPipeline::DiscoverTests(*program);

	const TypeCheckResult typeResult = CompilerPipeline::TypeCheck(*program);

	if (configuration.requireTests)
	{
		CompilerPipeline::ValidateTestCoverage(*program, discoveryResult);
	}

	const CodegenResult codegenResult = CompilerPipeline::Compile(*program, typeResult);

	VM vm;
	vm.LoadProgram(codegenResult.program);

	if (configuration.requireTests)
	{
		CompilerPipeline::RunTests(codegenResult, vm, configuration.report);
	}

	CompilerPipeline::RunProgram(codegenResult, vm);
}
