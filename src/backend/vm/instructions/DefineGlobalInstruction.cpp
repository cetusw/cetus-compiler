#include "DefineGlobalInstruction.h"
#include "../vm.h"
#include "src/backend/vm/types/Value.h"
#include <string>

InterpretResult DefineGlobalInstruction::Execute(VM& vm) const
{
	const Value nameValue = vm.ReadConstant();
	const std::string& name = nameValue.AsString();

	vm.DefineGlobal(name, vm.Pop());
	return InterpretResult::OK;
}
