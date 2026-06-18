#include "GetIndexInstruction.h"

#include "../objects/ObjArray.h"
#include "../objects/ObjSlice.h"
#include "../objects/ObjString.h"
#include "../vm.h"

namespace
{
int IndexableLength(const Value& value)
{
	if (value.IsString())
	{
		return static_cast<int>(value.AsString().length());
	}
	return value.IsArray() ? value.AsArray()->Length() : value.AsSlice()->Length();
}

Value IndexableGet(const Value& value, const int index)
{
	if (value.IsString())
	{
		return Value(std::make_shared<ObjString>(value.AsString().substr(static_cast<std::size_t>(index), 1)));
	}
	return value.IsArray() ? value.AsArray()->Get(index) : value.AsSlice()->Get(index);
}
} // namespace

// TODO нужно будет придумать как не делать Dereference каждый раз при снятии значения
InterpretResult GetIndexInstruction::Execute(VM& vm) const
{
	const Value indexValue = vm.Pop().Dereference();
	const Value indexableValue = vm.Pop().Dereference();
	if (!indexableValue.IsIndexable() || !indexValue.IsInt())
	{
		vm.SetRuntimeError("Index access expects indexable value and int index.");
		return InterpretResult::RUNTIME_ERROR;
	}

	const RuntimeInt index = indexValue.AsInt();
	const int length = IndexableLength(indexableValue);
	if (index < 0 || index >= length)
	{
		vm.SetRuntimeError("Index out of bounds.");
		return InterpretResult::RUNTIME_ERROR;
	}

	vm.Push(IndexableGet(indexableValue, static_cast<int>(index)));
	return InterpretResult::OK;
}
