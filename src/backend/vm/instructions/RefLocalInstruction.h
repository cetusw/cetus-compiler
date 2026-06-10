#pragma once

#include "Instruction.h"

class RefLocalInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
