#include "GetMemberInstruction.h"

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

InterpretResult GetMemberInstruction::Execute(VM& vm) const
{
	const Value fieldNameValue = vm.ReadConstant();
	const Value objectValue = vm.Pop().Dereference();
	if (!fieldNameValue.IsString())
	{
		return InterpretResult::RUNTIME_ERROR;
	}

	const std::string& fieldName = fieldNameValue.AsString();
	const auto object = ResolveStructObject(objectValue);
	if (!object)
	{
		return InterpretResult::RUNTIME_ERROR;
	}
	if (!object->HasField(fieldName))
	{
		return InterpretResult::RUNTIME_ERROR;
	}

	vm.Push(object->GetField(fieldName));
	return InterpretResult::OK;
}
