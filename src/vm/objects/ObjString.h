#pragma once
#include "Obj.h"
#include <string>

class ObjString final : public Obj
{
public:
	explicit ObjString(std::string string);
	[[nodiscard]] const std::string& GetData() const;

private:
	std::string m_data;
};
