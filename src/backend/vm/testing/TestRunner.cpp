#include "TestRunner.h"

#include "src/backend/vm/vm.h"

TestRunResult TestRunner::Run(const Program& program, VM& vm)
{
	TestRunResult result;
	const VM::GlobalSnapshot baselineGlobals = vm.SnapshotMutableGlobals();

	for (const TestDescriptor& test : program.testManifest.tests)
	{
		vm.RestoreMutableGlobals(baselineGlobals);
		vm.SetActiveTestName(test.name);
		if (vm.InterpretFunction(test.function) == InterpretResult::OK)
		{
			result.passedCount++;
			continue;
		}

		result.failedCount++;
		result.diagnostics.push_back("Test failed: " + test.name);
		result.firstFailure = TestFailure{ test.name, { "Test failed: " + test.name } };
		vm.RestoreMutableGlobals(baselineGlobals);
		vm.SetActiveTestName(std::nullopt);
		return result;
	}

	vm.RestoreMutableGlobals(baselineGlobals);
	vm.SetActiveTestName(std::nullopt);
	return result;
}
