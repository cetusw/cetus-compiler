#pragma once
#include "LR1ClosureCalculator.h"
#include "src/common/grammar/Grammar.h"
#include "types/LALR.h"
#include <map>
#include <memory>
#include <set>
#include <vector>

class LALRBuilder
{
public:
	explicit LALRBuilder(Grammar grammar);

	ParseTable Build();

private:
	void InitializeAugmentedGrammar();
	void BuildStatesGraph();
	void PropagateLookaheads();
	void ConstructResultTable();

	void ProcessStateTransitions(int stateIndex);
	void CreateTransition(int stateIndex, const Symbol& symbol, const LALRState& nextKernel);
	[[nodiscard]] LALRState ShiftDot(const LALRState& state, const Symbol& symbol) const;
	[[nodiscard]] std::set<LR0Item> ExtractKernel(const LALRState& state) const;

	Grammar m_grammar;
	Symbol m_augmentedStartSymbol;
	const Symbol m_endOfFileSymbol = Symbol("⊥", true);

	std::vector<Symbol> m_lhsRule;
	std::vector<std::vector<Symbol>> m_rhsRule;

	std::vector<LALRState> m_states;
	ParseTable m_table;

	std::unique_ptr<LR1ClosureCalculator> m_closureCalculator;

	std::map<std::set<LR0Item>, int> m_kernelToIndexMap;
	std::map<int, std::map<Symbol, int>> m_transitionMap;
};