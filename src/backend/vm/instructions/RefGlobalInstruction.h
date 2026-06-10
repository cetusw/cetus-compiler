#pragma once

#include "Instruction.h"

class RefGlobalInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
