#pragma once

#include "Instruction.h"

class NotInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
