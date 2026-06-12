#pragma once

#include "Instruction.h"

class RefMemberInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
