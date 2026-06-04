#include "RuntimeEnvironment.h"

#include "src/backend/vm/vm.h"

RuntimeEnvironment::RuntimeEnvironment(Bindings bindings)
	: m_bindings(std::move(bindings))
{
}

void RuntimeEnvironment::LoadInto(VM& vm) const
{
	for (const auto& [name, value] : m_bindings)
	{
		vm.DefineGlobal(name, value);
	}
}
