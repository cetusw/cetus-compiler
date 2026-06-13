#pragma once

#include "src/backend/vm/objects/ObjFunction.h"
#include <memory>
#include <vector>

struct Program
{
	std::vector<std::shared_ptr<ObjFunction>> functions;
	std::shared_ptr<ObjFunction> entryPoint;
	std::vector<std::shared_ptr<ObjFunction>> testFunctions;
};
