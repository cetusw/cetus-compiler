#include "SliceInstruction.h"

#include "../objects/ObjArray.h"
#include "../objects/ObjSlice.h"
#include "../vm.h"

#include <vector>

InterpretResult SliceInstruction::Execute(VM& vm) const
{
	const uint8_t length = vm.ReadByte();
	std::vector<Value> elements(length);
	for (int index = static_cast<int>(length) - 1; index >= 0; --index)
	{
		elements[index] = vm.Pop();
	}

	auto storage = std::make_shared<ObjArray>(std::move(elements));
	vm.Push(Value(std::make_shared<ObjSlice>(storage, 0, length, length)));
	return InterpretResult::OK;
}
