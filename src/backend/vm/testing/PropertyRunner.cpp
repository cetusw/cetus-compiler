#include "PropertyRunner.h"

#include "src/backend/vm/vm.h"
#include "src/frontend/testing/PropertyGenerator.h"

#include <random>

namespace
{
constexpr std::uint64_t PROPERTY_BASE_SEED = 0xC3715ULL;
constexpr int PROPERTY_RUN_COUNT = 25;
}

PropertyRunResult PropertyRunner::Run(const std::string& testName, const PropertyDescriptor& property, VM& vm)
{
	PropertyRunResult result;
	const VM::GlobalSnapshot baselineGlobals = vm.SnapshotMutableGlobals();
	std::mt19937_64 generator(PROPERTY_BASE_SEED ^ std::hash<std::string>{}(testName));

	for (int iteration = 0; iteration < PROPERTY_RUN_COUNT; ++iteration)
	{
		vm.RestoreMutableGlobals(baselineGlobals);
		vm.ClearRuntimeDiagnostics();

		std::vector<Value> arguments;
		arguments.reserve(property.parameters.size());
		for (const PropertyParameterDescriptor& parameter : property.parameters)
		{
			arguments.push_back(PropertyGenerator::Generate(parameter.generator, generator));
		}

		if (vm.InterpretFunction(property.function, arguments) == InterpretResult::OK)
		{
			continue;
		}

		result.passed = false;
		result.iteration = iteration;
		result.diagnostics = vm.ConsumeRuntimeDiagnostics();
		if (result.diagnostics.empty())
		{
			result.diagnostics.push_back("Property failed without diagnostic.");
		}
		vm.RestoreMutableGlobals(baselineGlobals);
		return result;
	}

	vm.RestoreMutableGlobals(baselineGlobals);
	return result;
}
