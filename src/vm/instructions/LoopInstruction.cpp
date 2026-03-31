#include "LoopInstruction.h"
#include "../vm.h"

InterpretResult LoopInstruction::Execute(VM& vm) const
{
	const uint16_t offset = vm.ReadShort();
	vm.Jump(-(offset + 3));
	return InterpretResult::OK;
}