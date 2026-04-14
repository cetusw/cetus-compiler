#pragma once
#include "src/common/grammar/Grammar.h"
#include "types/LALR.h"
#include <map>
#include <set>
#include <vector>

class LR1ClosureCalculator
{
public:
	explicit LR1ClosureCalculator(const Grammar& grammar);

	[[nodiscard]] LALRState ComputeClosure(const LALRState& itemSet) const;

private:
	void InitializeSets(const Grammar& grammar);
	void IndexRules(const Grammar& grammar);

	bool ProcessItemExpansion(const LR1Item& item, LALRState& nextState) const;
	static std::vector<Symbol> GetRuleTail(const LR1Item& item, const std::vector<Symbol>& rhs);
	[[nodiscard]] std::set<Symbol> GetLookaheads(std::vector<Symbol> sequence, const Symbol& lookahead) const;
	bool AddRuleToState(const Symbol& nonTerminal, const std::set<Symbol>& lookaheads, LALRState& state) const;

	std::vector<Symbol> m_lhsRule;
	std::vector<std::vector<Symbol>> m_rhsRule;
	std::map<Symbol, std::vector<int>> m_rulesByLeftHandSide;
	std::map<Symbol, std::set<Symbol>> m_firstSets;
	std::set<Symbol> m_nullableSets;
};