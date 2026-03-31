#include "ReturnInstruction.h"
#include "../vm.h"

InterpretResult ReturnInstruction::Execute(VM&) const
{
	return InterpretResult::OK_DONE;
}