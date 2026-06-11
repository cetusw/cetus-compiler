#include "SymbolTable.h"

#include <stdexcept>

SymbolTable::SymbolTable()
{
	m_scopes.emplace_back();
}

SymbolTable::SymbolTable(Bindings bindings)
{
	m_scopes.push_back(std::move(bindings));
}

void SymbolTable::EnterScope()
{
	m_scopes.emplace_back();
}

void SymbolTable::ExitScope()
{
	if (m_scopes.size() <= 1)
	{
		throw std::logic_error("Cannot exit global symbol scope.");
	}

	m_scopes.pop_back();
}

void SymbolTable::Define(SemanticSymbol symbol)
{
	if (m_scopes.empty())
	{
		m_scopes.emplace_back();
	}

	m_scopes.back()[symbol.name] = std::move(symbol);
}

const SemanticSymbol* SymbolTable::Resolve(const std::string& name) const
{
	for (auto scope = m_scopes.rbegin(); scope != m_scopes.rend(); ++scope)
	{
		const auto it = scope->find(name);
		if (it != scope->end())
		{
			return &it->second;
		}
	}

	return nullptr;
}

const SemanticSymbol* SymbolTable::ResolveInCurrentScope(const std::string& name) const
{
	if (m_scopes.empty())
	{
		return nullptr;
	}

	const auto it = m_scopes.back().find(name);
	if (it == m_scopes.back().end())
	{
		return nullptr;
	}

	return &it->second;
}

SemanticSymbol* SymbolTable::ResolveMutableInCurrentScope(const std::string& name)
{
	if (m_scopes.empty())
	{
		return nullptr;
	}

	const auto it = m_scopes.back().find(name);
	if (it == m_scopes.back().end())
	{
		return nullptr;
	}

	return &it->second;
}

const SymbolTable::Bindings& SymbolTable::GetBindings() const
{
	return m_scopes.front();
}
