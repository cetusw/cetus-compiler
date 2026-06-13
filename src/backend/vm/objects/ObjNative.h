#pragma once
#include "Obj.h"

class Value;

using NativeFn = Value (*)(int argCount, Value* args);

class ObjNative final : public Obj
{
public:
	ObjNative(NativeFn function, int returnArity);

	NativeFn m_function;
	int m_returnArity;
};
