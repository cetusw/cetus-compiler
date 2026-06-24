#pragma once

#include "src/frontend/ast/ASTNode.h"
#include <string>

class ExpressionFormatter
{
public:
	[[nodiscard]] static std::string Format(const ASTNode& node);
};
