#pragma once

#include "../Type.h"

#include <string>

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
};
