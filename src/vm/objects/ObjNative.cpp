#include "ObjNative.h"
#include "../types/value.h"

ObjNative::ObjNative(const NativeFn function)
	: Obj(ObjType::NATIVE)
	, m_function(function)
{
}