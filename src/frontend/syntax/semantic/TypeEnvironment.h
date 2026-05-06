#pragma once

#include "Type.h"
#include <optional>
#include <string>
#include <unordered_map>

class TypeEnvironment
{
public:
	using Bindings = std::unordered_map<std::string, Type>;

	TypeEnvironment() = default;
	explicit TypeEnvironment(Bindings bindings);

	[[nodiscard]] std::optional<Type> Resolve(const std::string& name) const;

private:
	Bindings m_bindings;
};
