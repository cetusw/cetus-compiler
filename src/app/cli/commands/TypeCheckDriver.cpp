#include "TypeCheckDriver.h"

#include "../../../frontend/semantic/rules/TypeRules.h"
#include "src/app/diagnostics/CompilerError.h"
#include "src/app/diagnostics/DiagnosticStage.h"
#include "src/app/pipeline/CompilerPipeline.h"
#include <iostream>

void TypeCheckDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw CompilerError(
			DiagnosticStage::Internal,
			"Input source file path is required for type checking.");
	}

	const std::unique_ptr<ProgramASTNode> program = CompilerPipeline::ParseFile(configuration);
	const TestDiscoveryResult discoveryResult = CompilerPipeline::DiscoverTests(*program);
	const TypeCheckResult typeResult = CompilerPipeline::TypeCheck(*program);
	if (configuration.requireTests)
	{
		CompilerPipeline::ValidateTestCoverage(*program, discoveryResult);
	}

	std::cout << "Type: " << TypeRules::ToString(typeResult.type) << std::endl;
}
