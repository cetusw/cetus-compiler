#include "disassembler/Disassembler.h"
#include "parser/BytecodeParser.h"
#include "vm.h"
#include <iostream>

int main(const int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: bytecode_parser <filename>" << std::endl;
		return 1;
	}

	const std::string filename = argv[1];
	Chunk chunk;

	if (!BytecodeParser::ParseFile(filename, chunk))
	{
		return 1;
	}

	std::printf("--- Disassembling Parsed Bytecode ---\n");
	disassembler::DisassembleChunk(chunk, "Parsed Program");
	std::printf("\n--- Execution ---\n");

	VM vm;
	const InterpretResult result = vm.Interpret(chunk);

	if (result == InterpretResult::OK)
	{
		std::printf("\nExecution Finished: OK\n");
	}
	else
	{
		std::printf("\nExecution Finished: RUNTIME ERROR\n");
	}

	return 0;
}