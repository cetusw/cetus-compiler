#include "StructInstruction.h"

#include "../objects/ObjStruct.h"
#include "../vm.h"

InterpretResult StructInstruction::Execute(VM& vm) const
{
	const Value typeNameValue = vm.ReadConstant();
	if (!typeNameValue.IsString())
	{
		vm.SetRuntimeError("Struct instruction expects string type name.");
		return InterpretResult::RUNTIME_ERROR;
	}

	const uint8_t fieldCount = vm.ReadByte();
	std::vector<std::string> fieldNames;
	fieldNames.reserve(fieldCount);
	for (uint8_t index = 0; index < fieldCount; ++index)
	{
		const Value fieldNameValue = vm.ReadConstant();
		if (!fieldNameValue.IsString())
		{
			vm.SetRuntimeError("Struct instruction expects string field name.");
			return InterpretResult::RUNTIME_ERROR;
		}
		fieldNames.push_back(fieldNameValue.AsString());
	}

	std::vector<std::pair<std::string, Value>> fields(fieldCount);
	for (int index = static_cast<int>(fieldCount) - 1; index >= 0; --index)
	{
		fields[static_cast<std::size_t>(index)] = {
			std::move(fieldNames[static_cast<std::size_t>(index)]),
			vm.Pop().Dereference()
		};
	}

	vm.Push(Value(std::make_shared<ObjStruct>(typeNameValue.AsString(), std::move(fields))));
	return InterpretResult::OK;
}
