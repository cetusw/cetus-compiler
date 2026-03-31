#include "PopInstruction.h"
#include "../vm.h"

InterpretResult PopInstruction::Execute(VM& vm) const
{
	vm.Pop();
	return InterpretResult::OK;
}