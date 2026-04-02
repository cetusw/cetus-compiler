#pragma once
#include "src/vm/objects/ObjFunction.h"

struct CallFrame
{
	std::shared_ptr<ObjFunction> function;
	const uint8_t* ip;
	Value* slots;
};
