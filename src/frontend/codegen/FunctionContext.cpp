#include "FunctionContext.h"

#include <ranges>

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
	m_localSlots.push_back(LocalSlot{ name, slot, 0 });
	if (slot >= m_nextLocalSlot)
	{
		m_nextLocalSlot = slot + 1;
	}
}

void FunctionContext::BeginScope()
{
	++m_scopeDepth;
}

int FunctionContext::EndScope()
{
	int localCount = 0;
	while (!m_localSlots.empty() && m_localSlots.back().scopeDepth == m_scopeDepth)
	{
		m_localSlots.pop_back();
		--m_nextLocalSlot;
		++localCount;
	}

	--m_scopeDepth;
	return localCount;
}

int FunctionContext::CurrentScopeDepth() const
{
	return m_scopeDepth;
}

int FunctionContext::DeclareLocal(const std::string& name)
{
	const int slot = m_nextLocalSlot;
	m_localSlots.push_back(LocalSlot{ name, slot, m_scopeDepth });
	++m_nextLocalSlot;
	return slot;
}

std::optional<int> FunctionContext::ResolveLocal(const std::string& name) const
{
	for (const auto & m_localSlot : std::views::reverse(m_localSlots))
	{
		if (m_localSlot.name == name)
		{
			return m_localSlot.slot;
		}
	}

	return std::nullopt;
}

int FunctionContext::CountLocalsAboveDepth(const int scopeDepth) const
{
	int localCount = 0;
	for (const LocalSlot& localSlot : m_localSlots)
	{
		if (localSlot.scopeDepth > scopeDepth)
		{
			++localCount;
		}
	}
	return localCount;
}
