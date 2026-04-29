#pragma once
#include "Instruction.h"

class CallInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
