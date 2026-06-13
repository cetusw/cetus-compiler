#include "ReturnInstruction.h"
#include "../vm.h"

InterpretResult ReturnInstruction::Execute(VM& vm) const
{
	const int returnArity = vm.GetCurrentFrame().function->returnArity;
	std::vector<Value> results(static_cast<std::size_t>(returnArity));
	for (int index = returnArity - 1; index >= 0; --index)
	{
		results[static_cast<std::size_t>(index)] = vm.Pop();
	}
	Value* calleeSlot = vm.GetCurrentFrame().slots;

	const int newFrameCount = vm.GetFrameCount() - 1;
	vm.SetFrameCount(newFrameCount);

	if (newFrameCount == 0)
	{
		return InterpretResult::OK_DONE;
	}

	vm.SetStackTop(calleeSlot);

	for (const Value& result : results)
	{
		vm.Push(result);
	}

	return InterpretResult::OK;
}
