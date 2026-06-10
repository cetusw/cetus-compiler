#include "RefLocalInstruction.h"

#include "../vm.h"
#include "src/backend/vm/objects/ObjRef.h"

InterpretResult RefLocalInstruction::Execute(VM& vm) const
{
	const uint8_t slot = vm.ReadByte();
	vm.Push(Value(std::make_shared<ObjRef>(vm.GetStackAddress(slot))));
	return InterpretResult::OK;
}
