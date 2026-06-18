#include "RefMemberInstruction.h"

#include "../objects/ObjPointer.h"
#include "../objects/ObjRef.h"
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

InterpretResult RefMemberInstruction::Execute(VM& vm) const
{
	const Value fieldNameValue = vm.ReadConstant();
	const Value objectValue = vm.Pop().Dereference();
	if (!fieldNameValue.IsString())
	{
		vm.SetRuntimeError("Member reference expects string field name.");
		return InterpretResult::RUNTIME_ERROR;
	}

	const auto object = ResolveStructObject(objectValue);
	if (!object)
	{
		vm.SetRuntimeError("Cannot access member through nil pointer.");
		return InterpretResult::RUNTIME_ERROR;
	}

	Value* field = object->GetFieldAddress(fieldNameValue.AsString());
	if (!field)
	{
		vm.SetRuntimeError("Struct field is not declared: " + object->GetTypeName() + "." + fieldNameValue.AsString());
		return InterpretResult::RUNTIME_ERROR;
	}

	vm.Push(Value(std::make_shared<ObjRef>(field)));
	return InterpretResult::OK;
}
