#pragma once

#include "Instruction.h"

class OverInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
