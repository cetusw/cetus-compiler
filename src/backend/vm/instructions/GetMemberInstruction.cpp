#include "GetMemberInstruction.h"

#include "../objects/ObjStruct.h"
#include "../vm.h"

InterpretResult GetMemberInstruction::Execute(VM& vm) const
{
	const Value fieldNameValue = vm.ReadConstant();
	const Value objectValue = vm.Pop().Dereference();
	if (!fieldNameValue.IsString() || !objectValue.IsStruct())
	{
		return InterpretResult::RUNTIME_ERROR;
	}

	const std::string& fieldName = fieldNameValue.AsString();
	const auto object = objectValue.AsStruct();
	if (!object->HasField(fieldName))
	{
		return InterpretResult::RUNTIME_ERROR;
	}

	vm.Push(object->GetField(fieldName));
	return InterpretResult::OK;
}
