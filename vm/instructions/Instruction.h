#pragma once
#include "types/InterpretResult.h"

class VM;

class Instruction
{
public:
	virtual ~Instruction() = default;
	virtual InterpretResult Execute(VM& vm) const = 0;
};