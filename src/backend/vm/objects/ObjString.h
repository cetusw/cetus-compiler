#pragma once
#include "Obj.h"
#include <string>

class ObjString final : public Obj
{
public:
	explicit ObjString(std::string string);
	[[nodiscard]] const std::string& GetData() const;
	[[nodiscard]] int Length() const;
	[[nodiscard]] std::string CharAt(int index) const;

private:
	std::string m_data;
};
