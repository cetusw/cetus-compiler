#include "SetGlobalInstruction.h"
#include "../vm.h"

InterpretResult SetGlobalInstruction::Execute(VM& vm) const
{
	const Value nameValue = vm.ReadConstant();
	const std::string& name = nameValue.AsString();

	if (!vm.SetGlobal(name, vm.Peek(0)))
	{
		std::fprintf(stderr, "Runtime Error: Undefined variable '%s'.\n", name.c_str());
		return InterpretResult::RUNTIME_ERROR;
	}

	return InterpretResult::OK;
}