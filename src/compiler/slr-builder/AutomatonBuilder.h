#pragma once

#include "../../common/grammar/Grammar.h"
#include "../../common/types/LR0Item.h"
#include "Action.h"
#include <map>
#include <set>
#include <vector>

using LR0State = std::set<LR0Item>;
using SLRTable = std::map<int, std::map<Symbol, Action>>;

class AutomatonBuilder
{
public:
	explicit AutomatonBuilder(Grammar grammar);

	SLRTable Build();

private:
	void AugmentGrammar();
	void InitializeInternalData();

	[[nodiscard]] LR0State ComputeClosure(const LR0State& itemSet) const;
	[[nodiscard]] LR0State ComputeGoTo(const LR0State& itemSet, const Symbol& symbol) const;

	int GetStateIndex(const LR0State& itemSet);
	void ProcessStateTransitions(int stateIndex);
	void FillReduceActions(int stateIndex, const std::map<Symbol, std::set<Symbol>>& followSets);

	Grammar m_grammar;
	Symbol m_augmentedStartSymbol;
	const Symbol m_endOfFileSymbol = { "⊥", true };

	std::vector<Symbol> m_ruleLeftHandSides;
	std::vector<std::vector<Symbol>> m_ruleRightHandSides;
	std::map<Symbol, std::vector<int>> m_rulesByLeftHandSide;

	std::vector<LR0State> m_states;
	SLRTable m_table;
};