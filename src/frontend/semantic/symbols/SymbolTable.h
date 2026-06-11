#pragma once

#include "SemanticSymbol.h"

#include <string>
#include <unordered_map>
#include <vector>

class SymbolTable
{
public:
	using Bindings = std::unordered_map<std::string, SemanticSymbol>;

	SymbolTable();
	explicit SymbolTable(Bindings bindings);

	void Define(SemanticSymbol symbol);
	void EnterScope();
	void ExitScope();
	[[nodiscard]] const SemanticSymbol* Resolve(const std::string& name) const;
	[[nodiscard]] const SemanticSymbol* ResolveInCurrentScope(const std::string& name) const;
	[[nodiscard]] SemanticSymbol* ResolveMutableInCurrentScope(const std::string& name);
	[[nodiscard]] const Bindings& GetBindings() const;

private:
	std::vector<Bindings> m_scopes;
};
