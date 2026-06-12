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

int ObjArray::Capacity() const
{
	return static_cast<int>(m_elements.capacity());
}

const Value& ObjArray::Get(const int index) const
{
	return m_elements.at(index);
}

Value* ObjArray::GetAddress(const int index)
{
	return &m_elements.at(index);
}

void ObjArray::Set(const int index, const Value& value)
{
	m_elements.at(index) = value;
}

void ObjArray::Push(const Value& value)
{
	m_elements.push_back(value);
}

void ObjArray::Reserve(const int capacity)
{
	m_elements.reserve(static_cast<std::size_t>(capacity));
}

void ObjArray::Resize(const int length)
{
	m_elements.resize(static_cast<std::size_t>(length));
}
