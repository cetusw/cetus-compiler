#include "SetIndexInstruction.h"

#include "../objects/ObjArray.h"
#include "../vm.h"

InterpretResult SetIndexInstruction::Execute(VM& vm) const
{
	const Value indexValue = vm.Pop().Dereference();
	const Value arrayValue = vm.Pop().Dereference();
	const Value assignedValue = vm.Pop().Dereference();
	if (!arrayValue.IsArray() || !indexValue.IsInt())
	{
		return InterpretResult::RUNTIME_ERROR;
	}

	const RuntimeInt index = indexValue.AsInt();
	const auto array = arrayValue.AsArray();
	if (index < 0 || index >= array->Length())
	{
		return InterpretResult::RUNTIME_ERROR;
	}

	array->Set(static_cast<int>(index), assignedValue);
	vm.Push(assignedValue);
	return InterpretResult::OK;
}
