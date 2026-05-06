#include "TypeEnvironment.h"

TypeEnvironment::TypeEnvironment(Bindings bindings)
	: m_bindings(std::move(bindings))
{
}

std::optional<Type> TypeEnvironment::Resolve(const std::string& name) const
{
	const auto it = m_bindings.find(name);
	if (it == m_bindings.end())
	{
		return std::nullopt;
	}

	return it->second;
}
