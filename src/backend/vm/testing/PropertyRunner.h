#pragma once

#include "TestRunResult.h"
#include "src/backend/vm/types/Program.h"

class VM;

struct PropertyRunResult
{
	bool passed = true;
	std::vector<std::string> diagnostics;
	int iteration = 0;
};

class PropertyRunner
{
public:
	[[nodiscard]] static PropertyRunResult Run(const std::string& testName, const PropertyDescriptor& property, VM& vm);
};
