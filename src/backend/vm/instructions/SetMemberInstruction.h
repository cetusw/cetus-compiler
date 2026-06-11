#pragma once

#include "Instruction.h"

class SetMemberInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
