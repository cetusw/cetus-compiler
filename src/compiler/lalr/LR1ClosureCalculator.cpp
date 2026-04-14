#include "LR1ClosureCalculator.h"
#include "src/utils/guding-set-provider/GuidingSetProvider.h"

LR1ClosureCalculator::LR1ClosureCalculator(const Grammar& grammar)
{
	InitializeSets(grammar);
	IndexRules(grammar);
}

void LR1ClosureCalculator::InitializeSets(const Grammar& grammar)
{
	GuidingSetProvider provider(grammar);
	provider.Execute();
	m_firstSets = provider.GetFirstSets();
	m_nullableSets = provider.GetNullableSets();
}

void LR1ClosureCalculator::IndexRules(const Grammar& grammar)
{
	const auto& rules = grammar.GetRules();
	for (int index = 0; index < static_cast<int>(rules.size()); ++index)
	{
		const auto& production = rules[index];
		m_lhsRule.push_back(production.GetLhs());
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
	const auto& rhs = m_rhsRule[item.core.ruleIndex];

	if (item.core.dotPos >= rhs.size() || rhs[item.core.dotPos].IsTerminal())
	{
		return false;
	}

	const std::vector ruleTail = GetRuleTail(item, rhs);
	const std::set<Symbol> lookaheads = GetLookaheads(ruleTail, item.lookahead);

	return AddRuleToState(rhs[item.core.dotPos], lookaheads, nextState);
}

std::vector<Symbol> LR1ClosureCalculator::GetRuleTail(const LR1Item& item, const std::vector<Symbol>& rhs)
{
	const auto ruleTailBegin = rhs.begin() + static_cast<int>(item.core.dotPos) + 1;
	const std::vector ruleTail(ruleTailBegin, rhs.end());
	return ruleTail;
}

std::set<Symbol> LR1ClosureCalculator::GetLookaheads(std::vector<Symbol> sequence, const Symbol& lookahead) const
{
	std::set<Symbol> result;
	sequence.push_back(lookahead);

	for (const auto& symbol : sequence)
	{
		if (symbol.IsTerminal())
		{
			result.insert(symbol);
			return result;
		}

		if (m_firstSets.contains(symbol))
		{
			for (const auto& firstSymbol : m_firstSets.at(symbol))
			{
				if (!firstSymbol.IsEpsilon())
				{
					result.insert(firstSymbol);
				}
			}
		}

		if (!m_nullableSets.contains(symbol))
		{
			break;
		}
	}

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