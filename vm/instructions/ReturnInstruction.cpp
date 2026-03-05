#include "ReturnInstruction.h"
#include "../vm.h"

InterpretResult ReturnInstruction::Execute(VM& vm) const
{
	return InterpretResult::OK_DONE;
}