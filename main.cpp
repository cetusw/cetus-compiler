#include "src/cli/CommandLineInterface.h"
#include "src/cli/driver/BytecodeExecutionDriver.h"
#include "src/cli/driver/TableGeneratorDriver.h"
#include "src/vm/vm.h"
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
			// ParserDriver::Execute(configuration);
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