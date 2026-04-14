#include "LALRBuilder.h"
#include <algorithm>

LALRBuilder::LALRBuilder(Grammar grammar)
	: m_grammar(std::move(grammar))
{
}

ParseTable LALRBuilder::Build()
{
	InitializeAugmentedGrammar();
	m_closureCalculator = std::make_unique<LR1ClosureCalculator>(m_grammar);

	BuildStatesGraph();
	PropagateLookaheads();
	ConstructResultTable();

	return m_table;
}

void LALRBuilder::InitializeAugmentedGrammar()
{
	const Symbol originalStart = m_grammar.GetStartSymbol();
	m_augmentedStartSymbol = Symbol(originalStart.GetValue() + "'", false);

	m_lhsRule.clear();
	m_rhsRule.clear();
	m_lhsRule.push_back(m_augmentedStartSymbol);
	m_rhsRule.push_back({ originalStart });

	for (const auto& rule : m_grammar.GetRules())
	{
		m_lhsRule.push_back(rule.GetLhs());
		m_rhsRule.push_back(rule.GetRhs());
	}

	m_grammar.SetRules({});
	for (size_t i = 0; i < m_lhsRule.size(); ++i)
	{
		m_grammar.AddRule(m_lhsRule[i], m_rhsRule[i]);
	}
	m_grammar.SetStartSymbol(m_augmentedStartSymbol);

	auto terminals = m_grammar.GetTerminals();
	terminals.insert(m_endOfFileSymbol);
	m_grammar.SetTerminals(terminals);
}

void LALRBuilder::BuildStatesGraph()
{
	LR1Item initialItem;
	initialItem.core = { 0, 0 };
	initialItem.lookahead = m_endOfFileSymbol;

	const LALRState initialKernel = { initialItem };
	m_states.push_back(initialKernel);
	m_kernelToIndexMap[ExtractKernel(initialKernel)] = 0;

	for (int i = 0; i < static_cast<int>(m_states.size()); ++i)
	{
		ProcessStateTransitions(i);
	}
}

void LALRBuilder::ProcessStateTransitions(const int stateIndex)
{
	const LALRState fullState = m_closureCalculator->ComputeClosure(m_states[stateIndex]);

	std::set<Symbol> grammarSymbols;
	for (const auto& item : fullState)
	{
		const auto& rightHandSide = m_rhsRule[item.core.ruleIndex];
		if (item.core.dotPos < rightHandSide.size())
		{
			grammarSymbols.insert(rightHandSide[item.core.dotPos]);
		}
	}

	for (const auto& symbol : grammarSymbols)
	{
		LALRState nextKernel = ShiftDot(fullState, symbol);
		CreateTransition(stateIndex, symbol, nextKernel);
	}
}

void LALRBuilder::PropagateLookaheads()
{
	bool changed = true;
	while (changed)
	{
		changed = false;
		for (auto const& [sourceIndex, targetMap] : m_transitionMap)
		{
			LALRState sourceFull = m_closureCalculator->ComputeClosure(m_states[sourceIndex]);

			for (auto const& [symbol, targetIndex] : targetMap)
			{
				LALRState shifted = ShiftDot(sourceFull, symbol);
				for (const auto& item : shifted)
				{
					if (m_states[targetIndex].insert(item).second)
					{
						changed = true;
					}
				}
			}
		}
	}
}

void LALRBuilder::ConstructResultTable()
{
	for (int i = 0; i < static_cast<int>(m_states.size()); ++i)
	{
		LALRState fullState = m_closureCalculator->ComputeClosure(m_states[i]);

		for (const auto& item : fullState)
		{
			const auto& rightHandSide = m_rhsRule[item.core.ruleIndex];

			if (item.core.dotPos < rightHandSide.size())
			{
				const Symbol& symbol = rightHandSide[item.core.dotPos];
				if (m_transitionMap.contains(i) && m_transitionMap.at(i).contains(symbol))
				{
					const int target = m_transitionMap.at(i).at(symbol);
					const ActionType type = symbol.IsTerminal() ? ActionType::SHIFT : ActionType::GOTO;
					m_table[i][symbol] = { type, target };
				}
			}
			else if (m_lhsRule[item.core.ruleIndex] == m_augmentedStartSymbol)
			{
				m_table[i][m_endOfFileSymbol] = { ActionType::ACCEPT, 0 };
			}
			else
			{
				m_table[i][item.lookahead] = { ActionType::REDUCE, item.core.ruleIndex };
			}
		}
	}
}

void LALRBuilder::CreateTransition(const int stateIndex, const Symbol& symbol, const LALRState& nextKernel)
{
	const std::set<LR0Item> core = ExtractKernel(nextKernel);
	int targetIndex;

	if (m_kernelToIndexMap.contains(core))
	{
		targetIndex = m_kernelToIndexMap[core];
		for (const auto& item : nextKernel)
		{
			m_states[targetIndex].insert(item);
		}
	}
	else
	{
		targetIndex = static_cast<int>(m_states.size());
		m_states.push_back(nextKernel);
		m_kernelToIndexMap[core] = targetIndex;
	}

	m_transitionMap[stateIndex][symbol] = targetIndex;
}

LALRState LALRBuilder::ShiftDot(const LALRState& state, const Symbol& symbol) const
{
	LALRState movedItems;
	for (const auto& item : state)
	{
		const auto& rightHandSide = m_rhsRule[item.core.ruleIndex];
		if (item.core.dotPos < rightHandSide.size() && rightHandSide[item.core.dotPos] == symbol)
		{
			movedItems.insert({ { item.core.ruleIndex, item.core.dotPos + 1 }, item.lookahead });
		}
	}
	return movedItems;
}

std::set<LR0Item> LALRBuilder::ExtractKernel(const LALRState& state) const
{
	std::set<LR0Item> kernel;
	for (const auto& item : state)
	{
		if (item.core.dotPos > 0 || m_lhsRule[item.core.ruleIndex] == m_augmentedStartSymbol)
		{
			kernel.insert(item.core);
		}
	}
	return kernel;
}