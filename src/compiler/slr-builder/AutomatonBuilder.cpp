#include "AutomatonBuilder.h"
#include "../../utils/guding-set-provider/GuidingSetProvider.h"
#include <algorithm>
#include <stdexcept>

AutomatonBuilder::AutomatonBuilder(Grammar grammar)
	: m_grammar(std::move(grammar))
{
}

void AutomatonBuilder::AugmentGrammar()
{
	const Symbol originalStart = m_grammar.GetStartSymbol();
	m_augmentedStartSymbol = Symbol(originalStart.GetValue() + "'", false);
	m_grammar.AddRule(m_augmentedStartSymbol, { originalStart });
	m_grammar.SetStartSymbol(m_augmentedStartSymbol);
}

void AutomatonBuilder::InitializeInternalData()
{
	const auto& rules = m_grammar.GetRules();
	m_ruleLeftHandSides.clear();
	m_ruleRightHandSides.clear();
	m_rulesByLeftHandSide.clear();

	for (int i = 0; i < static_cast<int>(rules.size()); ++i)
	{
		m_ruleLeftHandSides.push_back(rules[i].GetLhs());
		m_ruleRightHandSides.push_back(rules[i].GetRhs());
		m_rulesByLeftHandSide[rules[i].GetLhs()].push_back(i);
	}
}

LR0State AutomatonBuilder::ComputeClosure(const LR0State& itemSet) const
{
	LR0State closure = itemSet;
	bool changed = true;
	while (changed)
	{
		changed = false;
		LR0State nextClosure = closure;
		for (const auto& item : closure)
		{
			const auto& rightHandSide = m_ruleRightHandSides[item.ruleIndex];
			if (item.dotPos >= rightHandSide.size() || rightHandSide[item.dotPos].IsTerminal())
			{
				continue;
			}

			const Symbol& nonTerminal = rightHandSide[item.dotPos];
			for (const int ruleIndex : m_rulesByLeftHandSide.at(nonTerminal))
			{
				if (closure.insert({ ruleIndex, 0 }).second)
				{
					changed = true;
				}
			}
		}
	}
	return closure;
}

LR0State AutomatonBuilder::ComputeGoTo(const LR0State& itemSet, const Symbol& symbol) const
{
	LR0State movedItems;
	for (const auto& item : itemSet)
	{
		const auto& rightHandSide = m_ruleRightHandSides[item.ruleIndex];
		if (item.dotPos < rightHandSide.size() && rightHandSide[item.dotPos] == symbol)
		{
			movedItems.insert({ item.ruleIndex, item.dotPos + 1 });
		}
	}
	return ComputeClosure(movedItems);
}

int AutomatonBuilder::GetStateIndex(const LR0State& itemSet)
{
	for (int i = 0; i < static_cast<int>(m_states.size()); ++i)
	{
		if (m_states[i] == itemSet)
		{
			return i;
		}
	}
	m_states.push_back(itemSet);
	return static_cast<int>(m_states.size() - 1);
}

SLRTable AutomatonBuilder::Build()
{
	AugmentGrammar();
	InitializeInternalData();

	GuidingSetProvider provider(m_grammar);
	provider.Execute();
	const auto& followSets = provider.GetFollowSets();

	const int startRuleIndex = static_cast<int>(m_ruleLeftHandSides.size()) - 1;
	m_states.push_back(ComputeClosure({ { startRuleIndex, 0 } }));

	for (int i = 0; i < static_cast<int>(m_states.size()); ++i)
	{
		ProcessStateTransitions(i);
		FillReduceActions(i, followSets);
	}
	return m_table;
}

void AutomatonBuilder::ProcessStateTransitions(const int stateIndex)
{
	const LR0State currentState = m_states[stateIndex];
	std::set<Symbol> symbols;
	for (const auto& item : currentState)
	{
		const auto& rightHandSide = m_ruleRightHandSides[item.ruleIndex];
		if (item.dotPos < rightHandSide.size())
		{
			symbols.insert(rightHandSide[item.dotPos]);
		}
	}

	for (const auto& symbol : symbols)
	{
		const int nextIndex = GetStateIndex(ComputeGoTo(currentState, symbol));
		const ActionType type = symbol.IsTerminal() ? ActionType::SHIFT : ActionType::GOTO;
		m_table[stateIndex][symbol] = { type, nextIndex };
	}
}

void AutomatonBuilder::FillReduceActions(const int stateIndex, const std::map<Symbol, std::set<Symbol>>& followSets)
{
	const LR0State& currentState = m_states[stateIndex];
	for (const auto& item : currentState)
	{
		const auto& leftHandSide = m_ruleLeftHandSides[item.ruleIndex];
		if (item.dotPos != m_ruleRightHandSides[item.ruleIndex].size())
		{
			continue;
		}

		if (leftHandSide == m_augmentedStartSymbol)
		{
			m_table[stateIndex][m_endOfFileSymbol] = { ActionType::ACCEPT, 0 };
			continue;
		}

		for (const auto& terminal : followSets.at(leftHandSide))
		{
			if (m_table[stateIndex].contains(terminal) && m_table[stateIndex][terminal].type != ActionType::ERROR)
			{
				throw std::runtime_error("SLR Conflict at state " + std::to_string(stateIndex));
			}
			m_table[stateIndex][terminal] = { ActionType::REDUCE, item.ruleIndex };
		}
	}
}