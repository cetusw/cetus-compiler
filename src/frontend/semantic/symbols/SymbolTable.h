#pragma once

#include "SemanticSymbol.h"

#include <string>
#include <unordered_map>

class SymbolTable
{
public:
	using Bindings = std::unordered_map<std::string, SemanticSymbol>;

	SymbolTable() = default;
	explicit SymbolTable(Bindings bindings);

	void Define(SemanticSymbol symbol);
	[[nodiscard]] const SemanticSymbol* Resolve(const std::string& name) const;
	[[nodiscard]] const Bindings& GetBindings() const;

private:
	Bindings m_bindings;
};
