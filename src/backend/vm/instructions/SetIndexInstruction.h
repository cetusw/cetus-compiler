#pragma once

#include "Instruction.h"

class SetIndexInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
