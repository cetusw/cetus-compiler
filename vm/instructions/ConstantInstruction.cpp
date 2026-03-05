#include "ConstantInstruction.h"
#include "../vm.h"

InterpretResult ConstantInstruction::Execute(VM& vm) const
{
	const Value constant = vm.ReadConstant();
	vm.Push(constant);
	return InterpretResult::OK;
}