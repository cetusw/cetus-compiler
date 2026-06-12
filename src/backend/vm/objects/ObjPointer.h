#pragma once

#include "Obj.h"

class ObjPointer final : public Obj
{
public:
	explicit ObjPointer(HeapObject target);

	[[nodiscard]] bool IsNil() const;
	[[nodiscard]] const HeapObject& GetTarget() const;

private:
	HeapObject m_target;
};
