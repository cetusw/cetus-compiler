#include "JumpInstruction.h"
#include "../vm.h"

InterpretResult JumpInstruction::Execute(VM& vm) const
{
	const uint16_t offset = vm.ReadShort();
	vm.Jump(offset);
	return InterpretResult::OK;
}