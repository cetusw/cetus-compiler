#pragma once

#include "TestRunResult.h"
#include "src/backend/vm/types/Program.h"

class VM;

class TestRunner
{
public:
	[[nodiscard]] static TestRunResult Run(const Program& program, VM& vm);
};
