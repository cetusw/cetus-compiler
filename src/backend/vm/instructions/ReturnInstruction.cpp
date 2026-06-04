#include "ReturnInstruction.h"
#include "../vm.h"

InterpretResult ReturnInstruction::Execute(VM& vm) const
{
	const Value result = vm.Pop();
	Value* calleeSlot = vm.GetCurrentFrame().slots;

	const int newFrameCount = vm.GetFrameCount() - 1;
	vm.SetFrameCount(newFrameCount);

	if (newFrameCount == 0)
	{
		return InterpretResult::OK_DONE;
	}

	vm.SetStackTop(calleeSlot);

	vm.Push(result);

	return InterpretResult::OK;
}
