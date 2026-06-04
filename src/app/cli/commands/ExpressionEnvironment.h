#pragma once

#include "src/frontend/codegen/RuntimeEnvironment.h"
#include "src/frontend/syntax/semantic/TypeEnvironment.h"

class ExpressionEnvironment
{
public:
	[[nodiscard]] static TypeEnvironment CreateTypeEnvironment();
	[[nodiscard]] static RuntimeEnvironment CreateRuntimeEnvironment();
};
