#pragma once
#include "Instruction.h"

class PopInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
