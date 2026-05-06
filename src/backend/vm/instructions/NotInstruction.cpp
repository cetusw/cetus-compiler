#include "NotInstruction.h"

#include "../vm.h"

InterpretResult NotInstruction::Execute(VM& vm) const
{
	const Value operand = vm.Pop();
	const Value result = operand.LogicalNot();
	if (result.IsNull())
	{
		return InterpretResult::RUNTIME_ERROR;
	}

	vm.Push(result);
	return InterpretResult::OK;
}
