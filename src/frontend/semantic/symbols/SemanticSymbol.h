#pragma once

#include "../Type.h"

#include <string>
#include <vector>

enum class SemanticSymbolKind
{
	VARIABLE,
	FUNCTION
};

struct SemanticSymbol
{
	std::string name;
	Type type = Type::ERROR;
	SemanticSymbolKind kind = SemanticSymbolKind::VARIABLE;
	std::vector<Type> parameterTypes;
};
