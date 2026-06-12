#pragma once

#include "Instruction.h"

class PointerInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
