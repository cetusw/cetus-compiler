#pragma once
#include "Instruction.h"

class SetGlobalInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
