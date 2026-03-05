#include "NegateInstruction.h"
#include "../vm.h"

InterpretResult NegateInstruction::Execute(VM& vm) const
{
	if (!vm.Peek(0).IsNumber())
	{
		std::fprintf(stderr, "Operand must be a number.\n");
		return InterpretResult::RUNTIME_ERROR;
	}

	vm.Push(-vm.Pop());
	return InterpretResult::OK;
}