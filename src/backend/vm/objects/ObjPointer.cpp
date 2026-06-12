#include "ObjPointer.h"

#include <utility>

ObjPointer::ObjPointer(HeapObject target)
	: Obj(ObjType::POINTER)
	, m_target(std::move(target))
{
}

bool ObjPointer::IsNil() const
{
	return !m_target;
}

const HeapObject& ObjPointer::GetTarget() const
{
	return m_target;
}
