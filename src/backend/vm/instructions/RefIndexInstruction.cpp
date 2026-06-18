#include "RefIndexInstruction.h"

#include "../objects/ObjArray.h"
#include "../objects/ObjRef.h"
#include "../objects/ObjSlice.h"
#include "../vm.h"

InterpretResult RefIndexInstruction::Execute(VM& vm) const
{
	const Value indexValue = vm.Pop().Dereference();
	const Value sequenceValue = vm.Pop().Dereference();
	if (!indexValue.IsInt() || !sequenceValue.IsSequence())
	{
		vm.SetRuntimeError("Index reference expects sequence value and int index.");
		return InterpretResult::RUNTIME_ERROR;
	}

	const RuntimeInt index = indexValue.AsInt();
	if (index < 0)
	{
		vm.SetRuntimeError("Index out of bounds.");
		return InterpretResult::RUNTIME_ERROR;
	}

	Value* element = nullptr;
	if (sequenceValue.IsArray())
	{
		const auto array = sequenceValue.AsArray();
		if (index >= array->Length())
		{
			vm.SetRuntimeError("Index out of bounds.");
			return InterpretResult::RUNTIME_ERROR;
		}
		element = array->GetAddress(static_cast<int>(index));
	}
	else
	{
		const auto slice = sequenceValue.AsSlice();
		if (index >= slice->Length())
		{
			vm.SetRuntimeError("Index out of bounds.");
			return InterpretResult::RUNTIME_ERROR;
		}
		element = slice->GetAddress(static_cast<int>(index));
	}

	vm.Push(Value(std::make_shared<ObjRef>(element)));
	return InterpretResult::OK;
}
