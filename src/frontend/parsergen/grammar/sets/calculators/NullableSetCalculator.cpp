#include "NullableSetCalculator.h"

#include <algorithm>

NullableSet NullableSetCalculator::Calculate(const Grammar& grammar)
{
	return NullableSetCalculator(grammar).Execute();
}

NullableSetCalculator::NullableSetCalculator(const Grammar& grammar)
	: m_grammar(grammar)
{
}

NullableSet NullableSetCalculator::Execute()
{
	while (UpdateNullable())
	{
	}

	return std::move(m_nullable);
}

bool NullableSetCalculator::UpdateNullable()
{
	bool changed = false;

	for (const auto& rule : m_grammar.GetRules())
	{
		if (m_nullable.contains(rule.GetLhs()))
		{
			continue;
		}

		if (IsRuleNullable(rule))
		{
			m_nullable.insert(rule.GetLhs());
			changed = true;
		}
	}

	return changed;
}

bool NullableSetCalculator::IsRuleNullable(const Production& rule) const
{
	return std::ranges::all_of(rule.GetRhs(),
		[this](const Symbol& symbol) { return m_nullable.contains(symbol); });
}
