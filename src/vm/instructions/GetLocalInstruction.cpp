#include "GetLocalInstruction.h"
#include "../vm.h"

InterpretResult GetLocalInstruction::Execute(VM& vm) const
{
	const uint8_t slot = vm.ReadByte();
	vm.Push(vm.GetStack(slot));
	return InterpretResult::OK;
}