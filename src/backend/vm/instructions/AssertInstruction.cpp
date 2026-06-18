#include "AssertInstruction.h"

#include "../objects/ObjAssertionMetadata.h"
#include "../vm.h"
#include <string>

InterpretResult AssertInstruction::Execute(VM& vm) const
{
	const Value condition = vm.Pop().Dereference();
	const Value metadataValue = vm.ReadConstant();

	if (!condition.IsBool() || !metadataValue.IsAssertionMetadata())
	{
		vm.SetRuntimeError("Assertion expects bool condition.");
		return InterpretResult::RUNTIME_ERROR;
	}
	if (condition.AsBool())
	{
		return InterpretResult::OK;
	}

	const AssertionDescriptor& descriptor = metadataValue.AsAssertionMetadata()->GetDescriptor();
	std::string failureDiagnostic = "Assertion failed at line " + std::to_string(descriptor.sourceLine);
	if (!descriptor.sourceText.empty())
	{
		failureDiagnostic += ": " + descriptor.sourceText;
	}
	if (const std::optional<std::string>& activeTestName = vm.GetActiveTestName(); activeTestName.has_value())
	{
		vm.AddRuntimeDiagnostic(failureDiagnostic);
		return InterpretResult::RUNTIME_ERROR;
	}

	vm.SetRuntimeError(failureDiagnostic);
	return InterpretResult::RUNTIME_ERROR;
}
