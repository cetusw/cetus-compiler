#pragma once
#include "Instruction.h"

class LoopInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
