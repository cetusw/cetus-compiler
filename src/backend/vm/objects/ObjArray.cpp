#include "ObjArray.h"

#include <utility>

ObjArray::ObjArray(std::vector<Value> elements)
	: Obj(ObjType::ARRAY)
	, m_elements(std::move(elements))
{
}

int ObjArray::Length() const
{
	return static_cast<int>(m_elements.size());
}

const Value& ObjArray::Get(const int index) const
{
	return m_elements.at(index);
}

void ObjArray::Set(const int index, const Value& value)
{
	m_elements.at(index) = value;
}
