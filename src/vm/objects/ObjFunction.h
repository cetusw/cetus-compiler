#pragma once
#include "Obj.h"
#include "ObjString.h"
#include "../types/chunk.h"

class ObjFunction final : public Obj
{
public:
	ObjFunction();

	int arity;
	Chunk chunk;
	std::shared_ptr<ObjString> name;
};
