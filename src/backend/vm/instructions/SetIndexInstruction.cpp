#include "SetIndexInstruction.h"

#include "../objects/ObjArray.h"
#include "../objects/ObjSlice.h"
#include "../vm.h"

namespace
{
int SequenceLength(const Value& value)
{
	return value.IsArray() ? value.AsArray()->Length() : value.AsSlice()->Length();
}

void SequenceSet(const Value& value, const int index, const Value& assignedValue)
{
	if (value.IsArray())
	{
		value.AsArray()->Set(index, assignedValue);
		return;
	}

	value.AsSlice()->Set(index, assignedValue);
}
} // namespace

InterpretResult SetIndexInstruction::Execute(VM& vm) const
{
	const Value indexValue = vm.Pop().Dereference();
	const Value sequenceValue = vm.Pop().Dereference();
	const Value assignedValue = vm.Pop().Dereference();
	if (!sequenceValue.IsSequence() || !indexValue.IsInt())
	{
		return InterpretResult::RUNTIME_ERROR;
	}

	const RuntimeInt index = indexValue.AsInt();
	const int length = SequenceLength(sequenceValue);
	if (index < 0 || index >= length)
	{
		return InterpretResult::RUNTIME_ERROR;
	}

	SequenceSet(sequenceValue, static_cast<int>(index), assignedValue);
	vm.Push(assignedValue);
	return InterpretResult::OK;
}
