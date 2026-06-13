#include "AssertInstruction.h"

#include "../objects/ObjAssertionMetadata.h"
#include "../vm.h"
#include <cstdio>

InterpretResult AssertInstruction::Execute(VM& vm) const
{
	const Value condition = vm.Pop().Dereference();
	const Value metadataValue = vm.ReadConstant();
	const auto function = vm.GetCurrentFrame().function;

	if (!condition.IsBool() || !metadataValue.IsAssertionMetadata())
	{
		std::fprintf(stderr, "Assertion expects bool condition.\n");
		return InterpretResult::RUNTIME_ERROR;
	}
	if (condition.AsBool())
	{
		return InterpretResult::OK;
	}

	const AssertionDescriptor& descriptor = metadataValue.AsAssertionMetadata()->GetDescriptor();
	if (const std::optional<std::string>& activeTestName = vm.GetActiveTestName(); activeTestName.has_value())
	{
		std::fprintf(stderr, "Test failed: %s\n", activeTestName->c_str());
		std::fprintf(stderr, "Assertion failed at line %d", descriptor.sourceLine);
		if (!descriptor.sourceText.empty())
		{
			std::fprintf(stderr, ": %s", descriptor.sourceText.c_str());
		}
		std::fprintf(stderr, "\n");
		return InterpretResult::RUNTIME_ERROR;
	}

	std::fprintf(stderr, "Assertion failed in %s at line %d", function->name->GetData().c_str(),
		descriptor.sourceLine);
	if (!descriptor.sourceText.empty())
	{
		std::fprintf(stderr, ": %s", descriptor.sourceText.c_str());
	}
	std::fprintf(stderr, "\n");
	return InterpretResult::RUNTIME_ERROR;
}
