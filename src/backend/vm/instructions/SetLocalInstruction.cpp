#include "SetLocalInstruction.h"
#include "../vm.h"
#include "src/backend/vm/objects/ObjRef.h"

InterpretResult SetLocalInstruction::Execute(VM& vm) const
{
	const uint8_t slot = vm.ReadByte();
	const Value current = vm.GetStack(slot);
	if (current.IsRef())
	{
		current.AsRef()->Set(vm.Peek(0));
		return InterpretResult::OK;
	}

	vm.SetStack(slot, vm.Peek(0));
	return InterpretResult::OK;
}
