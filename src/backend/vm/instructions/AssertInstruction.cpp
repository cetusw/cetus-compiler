#include "AssertInstruction.h"

#include "../objects/ObjFunction.h"
#include "../vm.h"
#include <cstdio>
#include <string_view>

namespace
{
constexpr std::string_view TEST_PREFIX = "<test:";
constexpr char TEST_SUFFIX = '>';
}

InterpretResult AssertInstruction::Execute(VM& vm) const
{
	const Value condition = vm.Pop().Dereference();
	const auto function = vm.GetCurrentFrame().function;
	const std::string& functionName = function->name->GetData();

	if (!condition.IsBool())
	{
		std::fprintf(stderr, "Assertion expects bool condition.\n");
		return InterpretResult::RUNTIME_ERROR;
	}
	if (condition.AsBool())
	{
		return InterpretResult::OK;
	}

	if (functionName.starts_with(TEST_PREFIX) && !functionName.empty() && functionName.back() == TEST_SUFFIX)
	{
		const std::string_view testName(functionName.data() + TEST_PREFIX.size(), functionName.size() - TEST_PREFIX.size() - 1);
		std::fprintf(stderr, "Test failed: %.*s\n", static_cast<int>(testName.size()), testName.data());
		std::fprintf(stderr, "Assertion failed.\n");
		return InterpretResult::RUNTIME_ERROR;
	}

	std::fprintf(stderr, "Assertion failed in %s.\n", functionName.c_str());
	return InterpretResult::RUNTIME_ERROR;
}
