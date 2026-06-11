#pragma once

#include "Obj.h"
#include "src/backend/vm/types/Value.h"
#include <string>
#include <unordered_map>
#include <vector>

class ObjStruct final : public Obj
{
public:
	ObjStruct(std::string typeName, std::vector<std::pair<std::string, Value>> fields);

	[[nodiscard]] const std::string& GetTypeName() const;
	[[nodiscard]] bool HasField(const std::string& name) const;
	[[nodiscard]] const Value& GetField(const std::string& name) const;
	void SetField(const std::string& name, const Value& value);

private:
	std::string m_typeName;
	std::unordered_map<std::string, Value> m_fields;
};
