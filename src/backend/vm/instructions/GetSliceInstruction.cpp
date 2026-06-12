#include "GetSliceInstruction.h"

#include "../objects/ObjArray.h"
#include "../objects/ObjSlice.h"
#include "../vm.h"

namespace
{
int SequenceLength(const Value& value)
{
	return value.IsArray() ? value.AsArray()->Length() : value.AsSlice()->Length();
}

std::shared_ptr<ObjSlice> SliceSequence(const Value& value, const int start, const int end)
{
	if (value.IsArray())
	{
		return std::make_shared<ObjSlice>(value.AsArray(), start, end - start, SequenceLength(value) - start);
	}

	return value.AsSlice()->SubSlice(start, end);
}
} // namespace

InterpretResult GetSliceInstruction::Execute(VM& vm) const
{
	const uint8_t flags = vm.ReadByte();
	const bool hasStart = (flags & 0x01) != 0;
	const bool hasEnd = (flags & 0x02) != 0;

	RuntimeInt end = 0;
	if (hasEnd)
	{
		const Value endValue = vm.Pop().Dereference();
		if (!endValue.IsInt())
		{
			return InterpretResult::RUNTIME_ERROR;
		}
		end = endValue.AsInt();
	}

	RuntimeInt start = 0;
	if (hasStart)
	{
		const Value startValue = vm.Pop().Dereference();
		if (!startValue.IsInt())
		{
			return InterpretResult::RUNTIME_ERROR;
		}
		start = startValue.AsInt();
	}

	const Value sequenceValue = vm.Pop().Dereference();
	if (!sequenceValue.IsSequence())
	{
		return InterpretResult::RUNTIME_ERROR;
	}

	const int length = SequenceLength(sequenceValue);
	if (!hasEnd)
	{
		end = length;
	}
	if (start < 0 || end < start || end > length)
	{
		return InterpretResult::RUNTIME_ERROR;
	}

	vm.Push(Value(SliceSequence(sequenceValue, static_cast<int>(start), static_cast<int>(end))));
	return InterpretResult::OK;
}
