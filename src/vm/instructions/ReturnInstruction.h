#pragma once
#include "Instruction.h"

class ReturnInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};