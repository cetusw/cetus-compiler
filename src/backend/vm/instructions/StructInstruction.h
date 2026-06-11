#pragma once

#include "Instruction.h"

class StructInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
