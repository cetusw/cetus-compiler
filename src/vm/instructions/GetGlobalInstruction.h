#pragma once
#include "Instruction.h"

class GetGlobalInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};