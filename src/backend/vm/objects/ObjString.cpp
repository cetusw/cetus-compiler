#include "ObjString.h"
#include <utility>

ObjString::ObjString(std::string string)
	: Obj(ObjType::STRING)
	, m_data(std::move(string))
{
}

const std::string& ObjString::GetData() const
{
	return m_data;
}

int ObjString::Length() const
{
	return static_cast<int>(m_data.length());
}

std::string ObjString::CharAt(const int index) const
{
	return std::string(1, m_data.at(static_cast<std::size_t>(index)));
}
