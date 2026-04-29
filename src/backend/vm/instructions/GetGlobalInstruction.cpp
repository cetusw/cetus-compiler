#include "GetGlobalInstruction.h"
#include "../vm.h"

InterpretResult GetGlobalInstruction::Execute(VM& vm) const
{
	const Value nameValue = vm.ReadConstant();
	const std::string& name = nameValue.AsString();

	if (!vm.HasGlobal(name))
	{
		std::fprintf(stderr, "Runtime Error: Undefined variable '%s'.\n", name.c_str());
		return InterpretResult::RUNTIME_ERROR;
	}

	vm.Push(vm.GetGlobal(name));
	return InterpretResult::OK;
}