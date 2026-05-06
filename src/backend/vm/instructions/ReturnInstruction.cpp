#include "ReturnInstruction.h"
#include "../vm.h"

InterpretResult ReturnInstruction::Execute(VM& vm) const
{
	const Value result = vm.Pop();

	const int newFrameCount = vm.GetFrameCount() - 1;
	vm.SetFrameCount(newFrameCount);

	if (newFrameCount == 0)
	{
		// TODO Убрать, как только будет весь язык
		vm.SetLastResult(result);
		return InterpretResult::OK_DONE;
	}

	vm.SetStackTop(vm.GetCurrentFrame().slots);

	vm.Push(result);

	return InterpretResult::OK;
}
