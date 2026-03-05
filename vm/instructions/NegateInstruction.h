#pragma once
#include "Instruction.h"

class NegateInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
