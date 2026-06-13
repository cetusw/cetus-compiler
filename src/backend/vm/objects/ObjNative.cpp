#include "ObjNative.h"
#include "../types/Value.h"

ObjNative::ObjNative(const NativeFn function, const int returnArity)
	: Obj(ObjType::NATIVE)
	, m_function(function)
	, m_returnArity(returnArity)
{
}
