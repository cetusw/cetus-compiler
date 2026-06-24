#include "FirstSetResolver.h"

#include "src/frontend/parsergen/grammar/GrammarSpecialSymbols.h"

FirstSetResolver::FirstSetResolver(const NullableSet& nullable, const FirstSets& first)
	: m_nullable(nullable)
	, m_first(first)
{
}

std::set<Symbol> FirstSetResolver::GetFirstOfSequence(const std::span<const Symbol> sequence) const
{
	if (sequence.empty())
	{
		return { EpsilonSymbol() };
	}

	std::set<Symbol> result;

	for (const auto& symbol : sequence)
	{
		AddFirstWithoutEpsilon(result, symbol);
		if (!IsNullable(symbol))
		{
			return result;
		}
	}

	result.insert(EpsilonSymbol());
	return result;
}

std::set<Symbol> FirstSetResolver::GetFirstOfSequenceWithLookahead(
	const std::span<const Symbol> sequence,
	const Symbol& lookahead) const
{
	std::set<Symbol> result;

	for (const auto& symbol : sequence)
	{
		AddFirstWithoutEpsilon(result, symbol);
		if (!IsNullable(symbol))
		{
			return result;
		}
	}

	AddFirstWithoutEpsilon(result, lookahead);
	return result;
}

void FirstSetResolver::AddFirstWithoutEpsilon(std::set<Symbol>& target, const Symbol& symbol) const
{
	const auto firstIt = m_first.find(symbol);
	if (firstIt == m_first.end())
	{
		if (symbol.IsTerminal() && !symbol.IsEpsilon())
		{
			target.insert(symbol);
		}
		return;
	}

	for (const auto& firstSymbol : firstIt->second)
	{
		if (!firstSymbol.IsEpsilon())
		{
			target.insert(firstSymbol);
		}
	}
}

bool FirstSetResolver::IsNullable(const Symbol& symbol) const
{
	return m_nullable.contains(symbol);
}
