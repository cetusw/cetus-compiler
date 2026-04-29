#pragma once
#include "src/frontend/syntax/grammar/Grammar.h"
#include <map>
#include <set>
#include <vector>

struct GuidingSet
{
	Symbol lhs;
	std::vector<Symbol> rhs;
	std::set<Symbol> selectSet;
	int ruleIndex;
};

using GuidingSetsMap = std::vector<GuidingSet>;

class GuidingSetProvider
{
public:
	explicit GuidingSetProvider(Grammar g);

	GuidingSetsMap Execute();

	[[nodiscard]] const std::map<Symbol, std::set<Symbol>>& GetFirstSets() const;
	[[nodiscard]] const std::map<Symbol, std::set<Symbol>>& GetFollowSets() const;
	[[nodiscard]] const std::set<Symbol>& GetNullableSets() const;

private:
	void CalculateNullable();
	void CalculateFirstSets();
	void CalculateFollowSets();

	bool UpdateNullable();
	bool UpdateFirstSets();
	bool UpdateFollowSets();
	void ProcessRuleFollow(const Production& rule, bool& changed);

	std::set<Symbol> GetFirstOfSequence(const std::vector<Symbol>& sequence);
	GuidingSet CreateGuidingSet(const Production& rule, int index);

	Grammar m_grammar;
	std::set<Symbol> m_nullable;
	std::map<Symbol, std::set<Symbol>> m_first;
	std::map<Symbol, std::set<Symbol>> m_follow;

	const Symbol m_epsilon = { "e", true };
	const Symbol m_eof = { "⊥", true };
};
