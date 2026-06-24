#include "FirstSetCalculator.h"

#include "src/frontend/parsergen/grammar/GrammarSpecialSymbols.h"
#include "src/frontend/parsergen/grammar/sets/resolvers/FirstSetResolver.h"

FirstSets FirstSetCalculator::Calculate(const Grammar& grammar, const NullableSet& nullable)
{
	return FirstSetCalculator(grammar, nullable).Execute();
}

FirstSetCalculator::FirstSetCalculator(const Grammar& grammar, const NullableSet& nullable)
	: m_grammar(grammar)
	, m_nullable(nullable)
{
}

FirstSets FirstSetCalculator::Execute()
{
	InitializeTerminalFirstSets();
	InitializeEpsilonFirstSet();

	while (UpdateFirstSets())
	{
	}

	return std::move(m_first);
}

void FirstSetCalculator::InitializeTerminalFirstSets()
{
	for (const auto& terminal : m_grammar.GetTerminals())
	{
		m_first[terminal] = { terminal };
	}
}

void FirstSetCalculator::InitializeEpsilonFirstSet()
{
	const Symbol epsilon = EpsilonSymbol();
	m_first[epsilon] = { epsilon };
}

bool FirstSetCalculator::UpdateFirstSets()
{
	bool changed = false;
	const FirstSetResolver resolver(m_nullable, m_first);

	for (const auto& rule : m_grammar.GetRules())
	{
		auto& lhsFirst = m_first[rule.GetLhs()];
		const size_t sizeBefore = lhsFirst.size();

		const std::set<Symbol> rhsFirst = resolver.GetFirstOfSequence(rule.GetRhs());
		lhsFirst.insert(rhsFirst.begin(), rhsFirst.end());

		if (lhsFirst.size() > sizeBefore)
		{
			changed = true;
		}
	}

	return changed;
}
