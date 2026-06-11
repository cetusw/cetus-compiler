#pragma once

#include "Instruction.h"

class GetMemberInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};
