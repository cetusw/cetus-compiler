#pragma once
#include "Instruction.h"

class JumpIfFalseInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
