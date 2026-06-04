#include "FunctionContext.h"

FunctionContext::FunctionContext(std::shared_ptr<ObjFunction> function, std::optional<std::string>& error)
	: m_function(std::move(function))
	, m_emitter(m_function->chunk, error)
{
}

std::shared_ptr<ObjFunction> FunctionContext::GetFunction() const
{
	return m_function;
}

ObjFunction& FunctionContext::Function() const
{
	return *m_function;
}

BytecodeEmitter& FunctionContext::Emitter()
{
	return m_emitter;
}

void FunctionContext::RegisterParameter(const std::string& name, const int slot)
{
	m_localSlots[name] = slot;
}

std::optional<int> FunctionContext::ResolveLocal(const std::string& name) const
{
	const auto local = m_localSlots.find(name);
	if (local == m_localSlots.end())
	{
		return std::nullopt;
	}

	return local->second;
}
