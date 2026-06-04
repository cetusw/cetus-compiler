#pragma once

#include "src/backend/vm/types/Value.h"
#include <string>
#include <unordered_map>

class VM;

class RuntimeEnvironment
{
public:
	using Bindings = std::unordered_map<std::string, Value>;

	RuntimeEnvironment() = default;
	explicit RuntimeEnvironment(Bindings bindings);

	void LoadInto(VM& vm) const;

private:
	Bindings m_bindings;
};
