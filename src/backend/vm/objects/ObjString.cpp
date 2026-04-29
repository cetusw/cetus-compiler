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