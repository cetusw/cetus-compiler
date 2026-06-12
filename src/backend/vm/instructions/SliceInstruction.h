#pragma once

#include "Instruction.h"

class SliceInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
