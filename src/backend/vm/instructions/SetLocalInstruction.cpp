#include "SetLocalInstruction.h"
#include "../vm.h"

InterpretResult SetLocalInstruction::Execute(VM& vm) const
{
	const uint8_t slot = vm.ReadByte();
	vm.SetStack(slot, vm.Peek(0));
	return InterpretResult::OK;
}