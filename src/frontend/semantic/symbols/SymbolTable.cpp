#include "SymbolTable.h"

SymbolTable::SymbolTable(Bindings bindings)
	: m_bindings(std::move(bindings))
{
}

void SymbolTable::Define(SemanticSymbol symbol)
{
	m_bindings[symbol.name] = std::move(symbol);
}

const SemanticSymbol* SymbolTable::Resolve(const std::string& name) const
{
	const auto it = m_bindings.find(name);
	if (it == m_bindings.end())
	{
		return nullptr;
	}

	return &it->second;
}

const SymbolTable::Bindings& SymbolTable::GetBindings() const
{
	return m_bindings;
}
