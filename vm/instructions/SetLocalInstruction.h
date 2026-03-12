#pragma once
#include "Instruction.h"
#include "types/InterpretResult.h"

class SetLocalInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
