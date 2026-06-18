#include "GetGlobalInstruction.h"
#include "../vm.h"

InterpretResult GetGlobalInstruction::Execute(VM& vm) const
{
	const Value nameValue = vm.ReadConstant();
	const std::string& name = nameValue.AsString();

	if (!vm.HasGlobal(name))
	{
		vm.SetRuntimeError("Undefined global variable: " + name);
		return InterpretResult::RUNTIME_ERROR;
	}

	vm.Push(vm.GetGlobal(name).Dereference());
	return InterpretResult::OK;
}
