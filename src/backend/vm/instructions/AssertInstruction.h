#pragma once

#include "Instruction.h"

class AssertInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
