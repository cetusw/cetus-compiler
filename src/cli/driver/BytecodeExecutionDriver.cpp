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

	BytecodeParser parser;
	const std::shared_ptr<ObjFunction> mainFunction = parser.Parse(configuration.inputFilePath);

	if (!mainFunction)
	{
		std::fprintf(stderr, "Error: Could not find 'main' function or parsing failed.\n");
		return;
	}

	disassembler::DisassembleChunk(mainFunction->chunk, mainFunction->name->GetData());

	VM vm;
	const InterpretResult result = vm.Interpret(mainFunction);

	if (result == InterpretResult::OK)
	{
		std::printf("\nExecution Finished: OK\n");
		return;
	}
	std::printf("\nExecution Finished: RUNTIME ERROR\n");
}