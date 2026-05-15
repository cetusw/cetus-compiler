#pragma once

#include "../Type.h"

#include <string>

struct SemanticSymbol
{
	std::string name;
	Type type = Type::ERROR;
};
