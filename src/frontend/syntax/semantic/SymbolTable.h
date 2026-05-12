#pragma once

#include "Symbol.h"
#include <string>
#include <unordered_map>

class SymbolTable
{
public:
	using Bindings = std::unordered_map<std::string, Symbol>;

	SymbolTable() = default;
	explicit SymbolTable(Bindings bindings);

	void Define(Symbol symbol);
	[[nodiscard]] const Symbol* Resolve(const std::string& name) const;

private:
	Bindings m_bindings;
};
