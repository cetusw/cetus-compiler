#include "ObjNative.h"
#include "../types/Value.h"

ObjNative::ObjNative(const NativeFn function)
	: Obj(ObjType::NATIVE)
	, m_function(function)
{
}