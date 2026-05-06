#pragma once

#include "Type.h"
#include <optional>
#include <string>

struct TypeCheckResult
{
	Type type = Type::ERROR;
	std::optional<std::string> error;
};
