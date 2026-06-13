#include "ObjFunction.h"

ObjFunction::ObjFunction()
	: Obj(ObjType::FUNCTION)
	, arity(0)
	, returnArity(0)
	, name(nullptr)
{
}
