#include "AssertInstruction.h"

#include "../vm.h"
#include <cstdio>

InterpretResult AssertInstruction::Execute(VM& vm) const
{
	const Value condition = vm.Pop().Dereference();
	const Value lineValue = vm.ReadConstant();
	const Value sourceTextValue = vm.ReadConstant();
	const auto function = vm.GetCurrentFrame().function;

	if (!condition.IsBool() || !lineValue.IsInt() || !sourceTextValue.IsString())
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
		std::fprintf(stderr, "Assertion failed at line %lld", static_cast<long long>(lineValue.AsInt()));
		if (!sourceTextValue.AsString().empty())
		{
			std::fprintf(stderr, ": %s", sourceTextValue.AsString().c_str());
		}
		std::fprintf(stderr, "\n");
		return InterpretResult::RUNTIME_ERROR;
	}

	std::fprintf(stderr, "Assertion failed in %s at line %lld", function->name->GetData().c_str(),
		static_cast<long long>(lineValue.AsInt()));
	if (!sourceTextValue.AsString().empty())
	{
		std::fprintf(stderr, ": %s", sourceTextValue.AsString().c_str());
	}
	std::fprintf(stderr, "\n");
	return InterpretResult::RUNTIME_ERROR;
}
