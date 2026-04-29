#pragma once
#include "Instruction.h"

class DefineGlobalInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};