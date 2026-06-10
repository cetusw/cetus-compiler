#pragma once

#include "Obj.h"

class Value;

class ObjRef final : public Obj
{
public:
	explicit ObjRef(Value* target);

	[[nodiscard]] Value& Get() const;
	void Set(const Value& value) const;

private:
	Value* m_target;
};
