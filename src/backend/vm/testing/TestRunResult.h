#pragma once

#include <optional>
#include <string>
#include <vector>

struct TestFailure
{
	std::string testName;
	std::vector<std::string> diagnostics;
};

struct TestCaseResult
{
	std::string name;
	bool passed = false;
	std::vector<std::string> diagnostics;
};

struct TestRunResult
{
	int passedCount = 0;
	int failedCount = 0;
	std::vector<TestCaseResult> testCases;
	std::optional<TestFailure> firstFailure;
	std::vector<std::string> diagnostics;
};
