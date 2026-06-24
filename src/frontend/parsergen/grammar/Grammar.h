#pragma once

#include "Production.h"
#include "src/frontend/syntax/table/Symbol.h"
#include <set>
#include <vector>

class Grammar
{
public:
	void AddRule(const Symbol& lhs, const std::vector<Symbol>& rhs, SemanticTag semanticTag = SemanticTag::NONE);

	[[nodiscard]] const std::set<Symbol>& GetTerminals() const;
	[[nodiscard]] const std::set<Symbol>& GetNonTerminals() const;
	[[nodiscard]] const std::vector<Production>& GetRules() const;
	[[nodiscard]] const Symbol& GetStartSymbol() const;
	void SetTerminals(const std::set<Symbol>& terminals);
	void SetNonTerminals(const std::set<Symbol>& nonTerminals);
	void SetRules(const std::vector<Production>& rules);
	void SetStartSymbol(const Symbol& symbol);

private:
	std::set<Symbol> m_terminals;
	std::set<Symbol> m_nonTerminals;
	std::vector<Production> m_rules;
	Symbol m_startSymbol;
};
