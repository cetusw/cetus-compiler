#pragma once

#include "src/backend/vm/types/Program.h"
#include <optional>
#include <string>
#include <vector>

class VM;

struct TestFailure
{
	std::string testName;
	std::vector<std::string> diagnostics;
};

struct TestRunResult
{
	int passedCount = 0;
	int failedCount = 0;
	std::optional<TestFailure> firstFailure;
	std::vector<std::string> diagnostics;
};

class TestRunner
{
public:
	[[nodiscard]] static TestRunResult Run(const Program& program, VM& vm);
};
