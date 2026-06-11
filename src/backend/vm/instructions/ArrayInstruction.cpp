#include "ArrayInstruction.h"

#include "../objects/ObjArray.h"
#include "../vm.h"

#include <vector>

InterpretResult ArrayInstruction::Execute(VM& vm) const
{
	const uint8_t length = vm.ReadByte();
	std::vector<Value> elements(length);
	for (int index = static_cast<int>(length) - 1; index >= 0; --index)
	{
		elements[index] = vm.Pop();
	}

	vm.Push(Value(std::make_shared<ObjArray>(std::move(elements))));
	return InterpretResult::OK;
}
