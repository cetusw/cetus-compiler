#include "SetMemberInstruction.h"

#include "../objects/ObjPointer.h"
#include "../objects/ObjStruct.h"
#include "../vm.h"

namespace
{
std::shared_ptr<ObjStruct> ResolveStructObject(const Value& objectValue)
{
	if (objectValue.IsStruct())
	{
		return objectValue.AsStruct();
	}
	if (!objectValue.IsPointer())
	{
		return nullptr;
	}

	const auto pointer = objectValue.AsPointer();
	if (pointer->IsNil() || !pointer->GetTarget() || pointer->GetTarget()->GetType() != ObjType::STRUCT)
	{
		return nullptr;
	}

	return std::static_pointer_cast<ObjStruct>(pointer->GetTarget());
}
} // namespace

InterpretResult SetMemberInstruction::Execute(VM& vm) const
{
	const Value fieldNameValue = vm.ReadConstant();
	const Value objectValue = vm.Pop().Dereference();
	const Value assignedValue = vm.Pop().Dereference();
	if (!fieldNameValue.IsString())
	{
		vm.SetRuntimeError("Member assignment expects string field name.");
		return InterpretResult::RUNTIME_ERROR;
	}

	const std::string& fieldName = fieldNameValue.AsString();
	const auto object = ResolveStructObject(objectValue);
	if (!object)
	{
		vm.SetRuntimeError("Cannot assign member through nil pointer.");
		return InterpretResult::RUNTIME_ERROR;
	}
	if (!object->HasField(fieldName))
	{
		vm.SetRuntimeError("Struct field is not declared: " + object->GetTypeName() + "." + fieldName);
		return InterpretResult::RUNTIME_ERROR;
	}

	object->SetField(fieldName, assignedValue);
	vm.Push(assignedValue);
	return InterpretResult::OK;
}
