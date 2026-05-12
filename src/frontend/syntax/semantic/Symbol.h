#pragma once

#include "Type.h"
#include <string>

struct Symbol
{
	std::string name;
	Type type = Type::ERROR;
};
