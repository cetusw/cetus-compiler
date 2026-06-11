#include "GetIndexInstruction.h"

#include "../objects/ObjArray.h"
#include "../vm.h"

// TODO нужно будет придумать как не делать Dereference каждый раз при снятии значения
InterpretResult GetIndexInstruction::Execute(VM& vm) const
{
	const Value indexValue = vm.Pop().Dereference();
	const Value arrayValue = vm.Pop().Dereference();
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

	vm.Push(array->Get(static_cast<int>(index)));
	return InterpretResult::OK;
}
