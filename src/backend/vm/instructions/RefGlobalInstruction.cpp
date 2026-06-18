#include "RefGlobalInstruction.h"

#include "../vm.h"
#include "src/backend/vm/objects/ObjRef.h"

InterpretResult RefGlobalInstruction::Execute(VM& vm) const
{
	const Value nameValue = vm.ReadConstant();
	const std::string& name = nameValue.AsString();

	Value* global = vm.GetGlobalAddress(name);
	if (!global)
	{
		vm.SetRuntimeError("Undefined global variable: " + name);
		return InterpretResult::RUNTIME_ERROR;
	}

	vm.Push(Value(std::make_shared<ObjRef>(global)));
	return InterpretResult::OK;
}
