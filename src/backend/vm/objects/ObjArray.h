#pragma once

#include "Obj.h"
#include "src/backend/vm/types/Value.h"

#include <cstddef>
#include <vector>

class ObjArray final : public Obj
{
public:
	explicit ObjArray(std::vector<Value> elements);

	[[nodiscard]] int Length() const;
	[[nodiscard]] int Capacity() const;
	[[nodiscard]] const Value& Get(int index) const;
	[[nodiscard]] Value* GetAddress(int index);
	void Set(int index, const Value& value);
	void Push(const Value& value);
	void Reserve(int capacity);
	void Resize(int length);

private:
	std::vector<Value> m_elements;
};
