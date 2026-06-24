#pragma once

#include "src/frontend/parsergen/grammar/sets/GrammarSets.h"
#include <set>
#include <span>

class FirstSetResolver
{
public:
	FirstSetResolver(const NullableSet& nullable, const FirstSets& first);

	[[nodiscard]] std::set<Symbol> GetFirstOfSequence(std::span<const Symbol> sequence) const;
	[[nodiscard]] std::set<Symbol> GetFirstOfSequenceWithLookahead(
		std::span<const Symbol> sequence,
		const Symbol& lookahead) const;

private:
	void AddFirstWithoutEpsilon(std::set<Symbol>& target, const Symbol& symbol) const;
	[[nodiscard]] bool IsNullable(const Symbol& symbol) const;

	const NullableSet& m_nullable;
	const FirstSets& m_first;
};
