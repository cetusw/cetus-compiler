#include "PointerInstruction.h"

#include "../objects/ObjPointer.h"
#include "../vm.h"

InterpretResult PointerInstruction::Execute(VM& vm) const
{
	const Value targetValue = vm.Pop().Dereference();
	if (!targetValue.IsHeapObject())
	{
		vm.SetRuntimeError("Pointer instruction expects heap object target.");
		return InterpretResult::RUNTIME_ERROR;
	}

	vm.Push(Value(std::make_shared<ObjPointer>(targetValue.AsHeapObject())));
	return InterpretResult::OK;
}
