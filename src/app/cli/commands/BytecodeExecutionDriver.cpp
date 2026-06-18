#include "BytecodeExecutionDriver.h"
#include "src/app/diagnostics/CompilerError.h"
#include "src/app/diagnostics/DiagnosticStage.h"
#include "src/backend/vm/disassembler/Disassembler.h"
#include "src/backend/vm/parser/BytecodeParser.h"
#include "src/backend/vm/vm.h"

void BytecodeExecutionDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw CompilerError(
			DiagnosticStage::Internal,
			"Input bytecode file path is required for bytecode execution.");
	}

	BytecodeParser parser;
	const std::shared_ptr<ObjFunction> mainFunction = parser.Parse(configuration.inputFilePath);

	if (!mainFunction)
	{
		throw CompilerError(
			DiagnosticStage::Runtime,
			"Could not find 'main' function or bytecode parsing failed.");
	}

	disassembler::DisassembleChunk(mainFunction->chunk, mainFunction->name->GetData());

	VM vm;
	const InterpretResult result = vm.InterpretFunction(mainFunction);

	if (result == InterpretResult::OK)
	{
		std::printf("\nExecution Finished: OK\n");
		return;
	}
	throw CompilerError(
		DiagnosticStage::Runtime,
		vm.GetRuntimeErrorMessage());
}
