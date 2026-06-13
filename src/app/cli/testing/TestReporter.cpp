#include "TestReporter.h"

#include <ostream>

void TestReporter::Print(const TestRunResult& result, std::ostream& output)
{
	for (const TestCaseResult& testCase : result.testCases)
	{
		output << (testCase.passed ? "ok " : "fail ") << testCase.name << '\n';
		for (const std::string& diagnostic : testCase.diagnostics)
		{
			output << diagnostic << '\n';
		}
	}

	output << "tests: " << result.passedCount << " passed, " << result.failedCount << " failed\n";
}
