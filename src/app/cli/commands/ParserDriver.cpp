#include "ParserDriver.h"

#include "src/app/cli/utils/utils.h"
#include "src/frontend/lexical/LexicalAnalyzer.h"
#include "src/frontend/syntax/ast/AstDumper.h"
#include <iostream>
#include <stdexcept>

void ParserDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input source file path is required for parsing.");
	}

	const ParseResult result = Utils::ParseSourceFile(configuration);
	if (!result.success)
	{
		throw std::runtime_error(result.message);
	}

	std::cout << result.message << std::endl;
	if (configuration.mode == CompilerMode::PARSE_AST)
	{
		if (!result.ast)
		{
			throw std::runtime_error("AST was not produced for parsed input.");
		}

		AstDumper::Dump(*result.ast, std::cout);
	}
}
