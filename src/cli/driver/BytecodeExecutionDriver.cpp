#include "BytecodeExecutionDriver.h"
#include "src/vm/disassembler/Disassembler.h"
#include "src/vm/parser/BytecodeParser.h"
#include "src/vm/types/chunk.h"
#include "src/vm/vm.h"

void BytecodeExecutionDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input bytecode file path is required for bytecode execution.");
	}

	Chunk chunk;
	if (!BytecodeParser::ParseFile(configuration.inputFilePath, chunk))
	{
		return;
	}

	std::printf("--- Disassembling Parsed Bytecode ---\n");
	disassembler::DisassembleChunk(chunk, "Parsed Program");
	std::printf("\n--- Execution ---\n");

	VM vm;
	const InterpretResult result = vm.Interpret(chunk);

	if (result == InterpretResult::OK)
	{
		std::printf("\nExecution Finished: OK\n");
		return;
	}
	std::printf("\nExecution Finished: RUNTIME ERROR\n");
}