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
		std::fprintf(stderr, "Runtime Error: Undefined variable '%s'.\n", name.c_str());
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
