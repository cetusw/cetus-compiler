#include "ParserDriver.h"

#include "src/app/cli/utils/utils.h"
#include "src/app/diagnostics/CompilerError.h"
#include "src/app/diagnostics/DiagnosticStage.h"
#include "src/frontend/ast/debug/AstDumper.h"
#include <iostream>

void ParserDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw CompilerError(
			DiagnosticStage::Internal,
			"Input source file path is required for parsing.");
	}

	const ParseResult result = Utils::ParseSourceFile(configuration);
	if (!result.success)
	{
		throw CompilerError(
			result.stage,
			result.message);
	}

	std::cout << result.message << std::endl;
	if (configuration.mode == CompilerMode::PARSE_AST)
	{
		if (!result.ast)
		{
			throw CompilerError(
				DiagnosticStage::Internal,
				"AST was not produced for parsed input.");
		}

		AstDumper::Dump(*result.ast, std::cout);
	}
}
