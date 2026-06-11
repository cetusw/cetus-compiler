#pragma once

#include "Obj.h"
#include "src/backend/vm/types/Value.h"
#include <vector>

class ObjArray final : public Obj
{
public:
	explicit ObjArray(std::vector<Value> elements);

	[[nodiscard]] int Length() const;
	[[nodiscard]] const Value& Get(int index) const;

private:
	std::vector<Value> m_elements;
};
