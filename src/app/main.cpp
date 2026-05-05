#include "src/app/cli/CommandLineInterface.h"
#include "src/app/cli/commands/BytecodeExecutionDriver.h"
#include "src/app/cli/commands/ParserDriver.h"
#include "src/app/cli/commands/TableGeneratorDriver.h"
#include <iostream>

int main(const int argc, char* argv[])
{
	const Configuration configuration = CommandLineInterface::ParseArguments(argc, argv);

	try
	{
		switch (configuration.mode)
		{
		case CompilerMode::COMPILE:
			// CompilerDriver::Execute(configuration);
			break;
		case CompilerMode::PARSE:
		case CompilerMode::PARSE_AST:
			ParserDriver::Execute(configuration);
			break;
		case CompilerMode::GENERATE_TABLE:
			TableGeneratorDriver::Execute(configuration);
			break;
		case CompilerMode::RUN_VM:
			BytecodeExecutionDriver::Execute(configuration);
			break;
		case CompilerMode::HELP:
		default:
			break;
		}
	}
	catch (const std::exception& exception)
	{
		std::cerr << "Error: " << exception.what() << std::endl;
		return 1;
	}

	return 0;
}
