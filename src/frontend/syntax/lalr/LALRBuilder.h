#pragma once
#include "LR1ClosureCalculator.h"
#include "src/frontend/syntax/grammar/Grammar.h"
#include "types/LALR.h"
#include "types/ParserDefinition.h"
#include <map>
#include <memory>
#include <set>
#include <vector>

class LALRBuilder
{
public:
	explicit LALRBuilder(Grammar grammar);
	ParserDefinition Build();

private:
	struct IndexedRule
	{
		Symbol lhs;
		std::vector<Symbol> rhs;
	};

	void PrepareGrammar();
	void BuildStateGraph();
	void PropagateLookaheads();
	void FillParseTable();

	void DiscoverTransitions(int stateIndex);
	int GetOrCreateState(const LALRState& kernel);
	[[nodiscard]] LALRState ComputeNextKernel(const LALRState& state, const Symbol& symbol) const;
	[[nodiscard]] std::set<LR0Item> ExtractKernelCore(const LALRState& state) const;

	void AddAction(int state, const Symbol& symbol, Action action);
	[[nodiscard]] bool IsStartSymbol(const Symbol& lhs) const;

	Grammar m_grammar;
	Symbol m_augmentedStartSymbol;
	const Symbol m_eofSymbol = Symbol("⊥", true);

	std::vector<IndexedRule> m_rules;
	std::vector<LALRState> m_states;
	std::map<std::set<LR0Item>, int> m_kernelToId;
	std::map<int, std::map<Symbol, int>> m_transitions;

	ParseTable m_table;
	std::unique_ptr<LR1ClosureCalculator> m_closure;
};
