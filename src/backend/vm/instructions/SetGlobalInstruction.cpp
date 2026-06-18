#include "SetGlobalInstruction.h"
#include "../vm.h"
#include "src/backend/vm/objects/ObjRef.h"

InterpretResult SetGlobalInstruction::Execute(VM& vm) const
{
	const Value nameValue = vm.ReadConstant();
	const std::string& name = nameValue.AsString();

	Value* global = vm.GetGlobalAddress(name);
	if (!global)
	{
		vm.SetRuntimeError("Undefined global variable: " + name);
		return InterpretResult::RUNTIME_ERROR;
	}

	if (global->IsRef())
	{
		global->AsRef()->Set(vm.Peek(0));
		return InterpretResult::OK;
	}

	*global = vm.Peek(0);
	return InterpretResult::OK;
}
