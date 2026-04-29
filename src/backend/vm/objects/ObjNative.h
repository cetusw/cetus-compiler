#pragma once
#include "Obj.h"

class Value;

using NativeFn = Value (*)(int argCount, Value* args);

class ObjNative final : public Obj
{
public:
	explicit ObjNative(NativeFn function);

	NativeFn m_function;
};
