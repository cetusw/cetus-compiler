#pragma once

#include "src/frontend/parsergen/grammar/Grammar.h"
#include "src/frontend/parsergen/grammar/sets/GrammarSets.h"
#include <span>

class FollowSetCalculator
{
public:
	[[nodiscard]] static FollowSets Calculate(
		const Grammar& grammar,
		const NullableSet& nullable,
		const FirstSets& first);

private:
	FollowSetCalculator(
		const Grammar& grammar,
		const NullableSet& nullable,
		const FirstSets& first);

	[[nodiscard]] FollowSets Execute();
	void InitializeStartSymbolFollow();
	[[nodiscard]] bool UpdateFollowSets();
	void ProcessRuleFollow(const Production& rule, bool& changed);
	void ProcessFollowForNonTerminal(
		const Production& rule,
		size_t symbolIndex,
		bool& changed);
	[[nodiscard]] static std::span<const Symbol> GetSymbolsAfter(const std::vector<Symbol>& rhs, size_t symbolIndex);
	static void AddFirstOfTailToFollow(std::set<Symbol>& targetFollow, const std::set<Symbol>& tailFirst);
	static void AddLhsFollowIfTailNullable(
		std::set<Symbol>& targetFollow,
		const Symbol& lhs,
		const std::set<Symbol>& tailFirst,
		const FollowSets& follow);

	const Grammar& m_grammar;
	const NullableSet& m_nullable;
	const FirstSets& m_first;
	FollowSets m_follow;
};
