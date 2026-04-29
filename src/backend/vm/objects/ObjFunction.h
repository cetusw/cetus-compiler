#pragma once
#include "../types/Chunk.h"
#include "Obj.h"
#include "ObjString.h"

class ObjFunction final : public Obj
{
public:
	ObjFunction();

	int arity;
	Chunk chunk;
	std::shared_ptr<ObjString> name;
};
