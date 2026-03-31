#pragma once
#include "Instruction.h"

class ConstantInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};