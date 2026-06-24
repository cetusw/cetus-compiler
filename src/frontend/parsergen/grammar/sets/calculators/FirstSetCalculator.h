#pragma once

#include "src/frontend/parsergen/grammar/Grammar.h"
#include "src/frontend/parsergen/grammar/sets/GrammarSets.h"

class FirstSetCalculator
{
public:
	[[nodiscard]] static FirstSets Calculate(const Grammar& grammar, const NullableSet& nullable);

private:
	FirstSetCalculator(const Grammar& grammar, const NullableSet& nullable);

	[[nodiscard]] FirstSets Execute();
	void InitializeTerminalFirstSets();
	void InitializeEpsilonFirstSet();
	[[nodiscard]] bool UpdateFirstSets();

	const Grammar& m_grammar;
	const NullableSet& m_nullable;
	FirstSets m_first;
};
