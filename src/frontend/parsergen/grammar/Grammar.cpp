#include "Grammar.h"

#include <utility>

void Grammar::AddRule(const Symbol& lhs, const std::vector<Symbol>& rhs, const SemanticTag semanticTag)
{
	m_rules.emplace_back(lhs, rhs, semanticTag);
	m_nonTerminals.insert(lhs);
	for (const auto& symbol : rhs)
	{
		if (symbol.IsTerminal())
		{
			m_terminals.insert(symbol);
		}
		else
		{
			m_nonTerminals.insert(symbol);
		}
	}
}

const std::set<Symbol>& Grammar::GetTerminals() const
{
	return m_terminals;
}

const std::set<Symbol>& Grammar::GetNonTerminals() const
{
	return m_nonTerminals;
}

const std::vector<Production>& Grammar::GetRules() const
{
	return m_rules;
}

const Symbol& Grammar::GetStartSymbol() const
{
	return m_startSymbol;
}

void Grammar::SetTerminals(const std::set<Symbol>& terminals)
{
	m_terminals = terminals;
}
void Grammar::SetNonTerminals(const std::set<Symbol>& nonTerminals)
{
	m_nonTerminals = nonTerminals;
}

void Grammar::SetRules(const std::vector<Production>& rules)
{
	m_rules = rules;
}

void Grammar::SetStartSymbol(const Symbol& symbol)
{
	m_startSymbol = symbol;
}
