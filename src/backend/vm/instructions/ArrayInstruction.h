#pragma once

#include "Instruction.h"

class ArrayInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
