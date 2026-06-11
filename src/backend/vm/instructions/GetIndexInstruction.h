#pragma once

#include "Instruction.h"

class GetIndexInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
