#pragma once

#include "src/frontend/parsergen/grammar/Grammar.h"
#include "src/frontend/parsergen/grammar/sets/GrammarSets.h"

class NullableSetCalculator
{
public:
	[[nodiscard]] static NullableSet Calculate(const Grammar& grammar);

private:
	explicit NullableSetCalculator(const Grammar& grammar);

	[[nodiscard]] NullableSet Execute();
	[[nodiscard]] bool UpdateNullable();
	[[nodiscard]] bool IsRuleNullable(const Production& rule) const;

	const Grammar& m_grammar;
	NullableSet m_nullable;
};
