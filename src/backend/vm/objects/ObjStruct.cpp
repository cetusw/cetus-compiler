#include "ObjStruct.h"

#include <stdexcept>
#include <utility>

ObjStruct::ObjStruct(std::string typeName, std::vector<std::pair<std::string, Value>> fields)
	: Obj(ObjType::STRUCT)
	, m_typeName(std::move(typeName))
{
	for (auto& [name, value] : fields)
	{
		m_fields.emplace(std::move(name), std::move(value));
	}
}

const std::string& ObjStruct::GetTypeName() const
{
	return m_typeName;
}

bool ObjStruct::HasField(const std::string& name) const
{
	return m_fields.contains(name);
}

const Value& ObjStruct::GetField(const std::string& name) const
{
	return m_fields.at(name);
}

Value* ObjStruct::GetFieldAddress(const std::string& name)
{
	const auto it = m_fields.find(name);
	if (it == m_fields.end())
	{
		return nullptr;
	}

	return &it->second;
}

void ObjStruct::SetField(const std::string& name, const Value& value)
{
	const auto it = m_fields.find(name);
	if (it == m_fields.end())
	{
		throw std::out_of_range("Struct field does not exist.");
	}

	it->second = value;
}
