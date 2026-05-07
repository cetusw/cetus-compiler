#pragma once

#include <string>

enum class CompilerMode
{
	COMPILE,
	PARSE,
	PARSE_AST,
	TYPECHECK,
	RUN_EXPR,
	GENERATE_TABLE,
	RUN_VM,
	HELP
};

struct Configuration
{
	CompilerMode mode = CompilerMode::HELP;
	std::string inputFilePath;
	std::string outputFilePath = "out.csv";
	bool verbose = false;
};

class CommandLineInterface
{
public:
	static Configuration ParseArguments(int argumentCount, char* argumentValues[]);
	static void PrintHelp();

private:
	static Configuration MakeHelpConfiguration();
};
