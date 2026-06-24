#pragma once

#include "LALR.h"
#include "LR1ClosureCalculator.h"
#include "src/frontend/parsergen/grammar/Grammar.h"
#include "src/frontend/parsergen/grammar/sets/GrammarSets.h"
#include "src/frontend/syntax/table/PreparedGrammar.h"

#include <map>
#include <memory>
#include <set>
#include <vector>

class LALRBuilder
{
public:
	explicit LALRBuilder(Grammar grammar);

	PreparedGrammar Build();

private:
	struct IndexedRule
	{
		Symbol lhs;
		std::vector<Symbol> rhs;
		SemanticTag semanticTag = SemanticTag::NONE;
	};

	void PrepareGrammar();
	void LoadRulesFromGrammar();
	void EnsureBuildCanStart() const;
	void BuildInitialState();
	void BuildStateGraph();
	void PropagateLookaheads();
	void FillParseTable();
	[[nodiscard]] PreparedGrammar BuildPreparedGrammar() const;

	void DiscoverTransitions(int stateIndex);
	[[nodiscard]] std::set<Symbol> CollectNextSymbols(const LALRState& fullState) const;
	int GetOrCreateState(const LALRState& kernel);
	int MergeOrCreateState(const LALRState& kernel);
	[[nodiscard]] LALRState ComputeNextKernel(const LALRState& state, const Symbol& symbol) const;
	[[nodiscard]] LR0StateCore ExtractKernelCore(const LALRState& state) const;

	void AddAction(int state, const Symbol& symbol, Action action);
	void AddShiftGotoAction(int stateIndex, const LR1Item& item, const IndexedRule& rule);
	void AddReduceOrAcceptAction(int stateIndex, const LR1Item& item, const IndexedRule& rule);
	[[noreturn]] static void ReportConflict(int state, const Symbol& symbol, Action oldAction, Action newAction);
	[[nodiscard]] bool IsStartSymbol(const Symbol& lhs) const;

	Grammar m_grammar;
	Symbol m_augmentedStartSymbol;
	Symbol m_eofSymbol;
	GrammarSets m_grammarSets;

	std::vector<IndexedRule> m_rules;
	std::vector<LALRState> m_states;
	std::map<LR0StateCore, int> m_kernelToId;
	std::map<int, std::map<Symbol, int>> m_transitions;

	ParseTable m_table;
	std::unique_ptr<LR1ClosureCalculator> m_closure;
	bool m_isBuilt = false;
};
