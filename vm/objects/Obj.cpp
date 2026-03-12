#include "Obj.h"

Obj::Obj(const ObjType type)
	: m_type(type)
{
}

ObjType Obj::GetType() const
{
	return m_type;
}

void Obj::SetType(const ObjType type)
{
	m_type = type;
}
