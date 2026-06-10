#pragma once

#include "../Type.h"

#include <string>
#include <vector>

struct ParameterSignature
{
	Type type = Type::ERROR;
	bool isPointer = false;
};

enum class SemanticSymbolKind
{
	VARIABLE,
	FUNCTION,
	BUILTIN_FUNCTION
};

struct SemanticSymbol
{
	std::string name;
	Type type = Type::ERROR;
	SemanticSymbolKind kind = SemanticSymbolKind::VARIABLE;
	std::vector<ParameterSignature> parameters;
};
