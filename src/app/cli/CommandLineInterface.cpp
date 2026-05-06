#include "CommandLineInterface.h"
#include <iostream>

Configuration CommandLineInterface::ParseArguments(const int argumentCount, char* argumentValues[])
{
	Configuration configuration;

	if (argumentCount < 2)
	{
		configuration.mode = CompilerMode::HELP;
		return configuration;
	}

	const std::string command = argumentValues[1];

	if (command == "--compile")
	{
		configuration.mode = CompilerMode::COMPILE;
	}
	else if (command == "--parse")
	{
		configuration.mode = CompilerMode::PARSE;
	}
	else if (command == "--parse-ast")
	{
		configuration.mode = CompilerMode::PARSE_AST;
	}
	else if (command == "--typecheck")
	{
		configuration.mode = CompilerMode::TYPECHECK;
	}
	else if (command == "--table")
	{
		configuration.mode = CompilerMode::GENERATE_TABLE;
	}
	else if (command == "--run")
	{
		configuration.mode = CompilerMode::RUN_VM;
	}

	if (argumentCount > 2)
	{
		configuration.inputFilePath = argumentValues[2];
	}
	if (argumentCount > 3)
	{
		configuration.outputFilePath = argumentValues[3];
	}

	return configuration;
}

void CommandLineInterface::PrintHelp()
{
	std::cout << "Usage: cetus <command> <file>\n"
			  << "Commands:\n"
			  << "  compile <file>   Compile source to bytecode\n"
			  << "  parse <file>     Check syntax only\n"
			  << "  parse-ast <file> Check syntax and print AST\n"
			  << "  typecheck <file> Check syntax and infer expression type\n"
			  << "  table <file>     Generate SLR(1) table from grammar\n"
			  << "  run <file>       Execute bytecode in VM\n";
}
