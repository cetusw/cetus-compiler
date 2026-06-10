#include "ObjRef.h"

#include "src/backend/vm/types/Value.h"

ObjRef::ObjRef(Value* target)
	: Obj(ObjType::REF)
	, m_target(target)
{
}

Value& ObjRef::Get() const
{
	return *m_target;
}

void ObjRef::Set(const Value& value) const
{
	*m_target = value;
}
