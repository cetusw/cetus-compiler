#include "TestRunner.h"

#include "src/backend/vm/vm.h"

TestRunResult TestRunner::Run(const Program& program, VM& vm)
{
	TestRunResult result;
	const VM::GlobalSnapshot baselineGlobals = vm.SnapshotMutableGlobals();

	for (const TestDescriptor& test : program.testManifest.tests)
	{
		vm.RestoreMutableGlobals(baselineGlobals);
		vm.ClearRuntimeDiagnostics();
		vm.SetActiveTestName(test.name);
		if (vm.InterpretFunction(test.function) == InterpretResult::OK)
		{
			result.passedCount++;
			result.testCases.push_back(TestCaseResult{ test.name, true, {} });
			continue;
		}

		result.failedCount++;
		std::vector<std::string> diagnostics = vm.ConsumeRuntimeDiagnostics();
		if (diagnostics.empty())
		{
			diagnostics.push_back("Test failed without diagnostic.");
		}
		result.diagnostics.insert(result.diagnostics.end(), diagnostics.begin(), diagnostics.end());
		result.testCases.push_back(TestCaseResult{ test.name, false, diagnostics });
		result.firstFailure = TestFailure{ test.name, diagnostics };
		vm.RestoreMutableGlobals(baselineGlobals);
		vm.SetActiveTestName(std::nullopt);
		return result;
	}

	vm.RestoreMutableGlobals(baselineGlobals);
	vm.SetActiveTestName(std::nullopt);
	return result;
}
