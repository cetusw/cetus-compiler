#include "JumpIfFalseInstruction.h"
#include "../vm.h"

InterpretResult JumpIfFalseInstruction::Execute(VM& vm) const
{
	const uint16_t offset = vm.ReadShort();
	if (vm.Peek(0).IsFalsey())
	{
		vm.Jump(offset);
	}
	return InterpretResult::OK;
}