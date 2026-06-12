#include "GetIndexInstruction.h"

#include "../objects/ObjArray.h"
#include "../objects/ObjSlice.h"
#include "../vm.h"

namespace
{
int SequenceLength(const Value& value)
{
	return value.IsArray() ? value.AsArray()->Length() : value.AsSlice()->Length();
}

const Value& SequenceGet(const Value& value, const int index)
{
	return value.IsArray() ? value.AsArray()->Get(index) : value.AsSlice()->Get(index);
}
} // namespace

// TODO нужно будет придумать как не делать Dereference каждый раз при снятии значения
InterpretResult GetIndexInstruction::Execute(VM& vm) const
{
	const Value indexValue = vm.Pop().Dereference();
	const Value sequenceValue = vm.Pop().Dereference();
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

	vm.Push(SequenceGet(sequenceValue, static_cast<int>(index)));
	return InterpretResult::OK;
}
