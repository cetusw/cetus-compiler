#include "OverInstruction.h"

#include "../vm.h"

InterpretResult OverInstruction::Execute(VM& vm) const
{
	vm.Push(vm.Peek(1));
	return InterpretResult::OK;
}
