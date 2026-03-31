#pragma once

#include <string>

enum class CompilerMode
{
	COMPILE,
	PARSE,
	GENERATE_TABLE,
	RUN_VM,
	HELP
};

struct Configuration
{
	CompilerMode mode = CompilerMode::HELP;
	std::string inputFilePath;
	std::string outputFilePath = "out.bin";
	bool verbose = false;
};

class CommandLineInterface
{
public:
	static Configuration ParseArguments(int argumentCount, char* argumentValues[]);

private:
	static void PrintHelp();
};