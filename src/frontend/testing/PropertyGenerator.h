#pragma once

#include "src/backend/vm/types/Program.h"
#include "src/backend/vm/types/Value.h"
#include "src/frontend/semantic/Type.h"
#include <optional>
#include <random>

class PropertyGenerator
{
public:
	[[nodiscard]] static std::optional<PropertyGeneratorKind> ResolveKind(const TypeDescriptor& type);
	[[nodiscard]] static bool Supports(const TypeDescriptor& type);
	[[nodiscard]] static Value Generate(PropertyGeneratorKind kind, std::mt19937_64& generator);
};
