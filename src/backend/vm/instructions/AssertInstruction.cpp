#include "AssertInstruction.h"

#include "../vm.h"
#include <cstdio>

InterpretResult AssertInstruction::Execute(VM& vm) const
{
	const Value condition = vm.Pop().Dereference();
	const auto function = vm.GetCurrentFrame().function;

	if (!condition.IsBool())
	{
		std::fprintf(stderr, "Assertion expects bool condition.\n");
		return InterpretResult::RUNTIME_ERROR;
	}
	if (condition.AsBool())
	{
		return InterpretResult::OK;
	}

	if (const std::optional<std::string>& activeTestName = vm.GetActiveTestName(); activeTestName.has_value())
	{
		std::fprintf(stderr, "Test failed: %s\n", activeTestName->c_str());
		std::fprintf(stderr, "Assertion failed.\n");
		return InterpretResult::RUNTIME_ERROR;
	}

	std::fprintf(stderr, "Assertion failed in %s.\n", function->name->GetData().c_str());
	return InterpretResult::RUNTIME_ERROR;
}
