#include "LR1ClosureCalculator.h"

#include "src/frontend/parsergen/grammar/GrammarSpecialSymbols.h"
#include "src/frontend/parsergen/grammar/sets/resolvers/FirstSetResolver.h"

#include <utility>

LR1ClosureCalculator::LR1ClosureCalculator(const Grammar& grammar, GrammarSets sets)
	: m_sets(std::move(sets))
{
	IndexRules(grammar);
}

void LR1ClosureCalculator::IndexRules(const Grammar& grammar)
{
	const auto& rules = grammar.GetRules();
	for (int index = 0; index < static_cast<int>(rules.size()); ++index)
	{
		const auto& production = rules[index];
		m_rhsRule.push_back(production.GetRhs());
		m_rulesByLeftHandSide[production.GetLhs()].push_back(index);
	}
}

LALRState LR1ClosureCalculator::ComputeClosure(const LALRState& itemSet) const
{
	LALRState closure = itemSet;

	bool changed = true;
	while (changed)
	{
		changed = false;
		LALRState nextStep = closure;

		for (const auto& item : closure)
		{
			if (ProcessItemExpansion(item, nextStep))
			{
				changed = true;
			}
		}
		closure = nextStep;
	}

	return closure;
}

bool LR1ClosureCalculator::ProcessItemExpansion(const LR1Item& item, LALRState& nextState) const
{
	const auto& rhs = m_rhsRule[item.core.productionIndex];

	if (item.core.dotPosition >= rhs.size() || rhs[item.core.dotPosition].IsTerminal())
	{
		return false;
	}

	const std::span<const Symbol> ruleTail = GetRuleTail(item, rhs);
	const std::set<Symbol> lookaheads = GetLookaheads(ruleTail, item.lookahead);

	return AddRuleToState(rhs[item.core.dotPosition], lookaheads, nextState);
}

std::span<const Symbol> LR1ClosureCalculator::GetRuleTail(const LR1Item& item, const std::vector<Symbol>& rhs)
{
	return std::span<const Symbol>(rhs).subspan(item.core.dotPosition + 1);
}

std::set<Symbol> LR1ClosureCalculator::GetLookaheads(std::span<const Symbol> sequence, const Symbol& lookahead) const
{
	const FirstSetResolver resolver(m_sets.nullable, m_sets.first);
	std::set<Symbol> result = resolver.GetFirstOfSequenceWithLookahead(sequence, lookahead);
	result.erase(EpsilonSymbol());
	return result;
}

bool LR1ClosureCalculator::AddRuleToState(const Symbol& nonTerminal, const std::set<Symbol>& lookaheads, LALRState& state) const
{
	bool changed = false;
	for (const int ruleIndex : m_rulesByLeftHandSide.at(nonTerminal))
	{
		for (const auto& lookaheadSymbol : lookaheads)
		{
			if (state.insert({ { ruleIndex, 0 }, lookaheadSymbol }).second)
			{
				changed = true;
			}
		}
	}

	return changed;
}
