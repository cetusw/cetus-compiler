#include "FollowSetCalculator.h"

#include "src/frontend/parsergen/grammar/GrammarSpecialSymbols.h"
#include "src/frontend/parsergen/grammar/sets/resolvers/FirstSetResolver.h"

FollowSets FollowSetCalculator::Calculate(
	const Grammar& grammar,
	const NullableSet& nullable,
	const FirstSets& first)
{
	return FollowSetCalculator(grammar, nullable, first).Execute();
}

FollowSetCalculator::FollowSetCalculator(
	const Grammar& grammar,
	const NullableSet& nullable,
	const FirstSets& first)
	: m_grammar(grammar)
	, m_nullable(nullable)
	, m_first(first)
{
}

FollowSets FollowSetCalculator::Execute()
{
	InitializeStartSymbolFollow();

	while (UpdateFollowSets())
	{
	}

	return std::move(m_follow);
}

void FollowSetCalculator::InitializeStartSymbolFollow()
{
	m_follow[m_grammar.GetStartSymbol()].insert(EofSymbol());
}

bool FollowSetCalculator::UpdateFollowSets()
{
	bool changed = false;

	for (const auto& rule : m_grammar.GetRules())
	{
		ProcessRuleFollow(rule, changed);
	}

	return changed;
}

void FollowSetCalculator::ProcessRuleFollow(const Production& rule, bool& changed)
{
	const auto& rhs = rule.GetRhs();
	for (size_t symbolIndex = 0; symbolIndex < rhs.size(); ++symbolIndex)
	{
		if (rhs[symbolIndex].IsTerminal())
		{
			continue;
		}

		ProcessFollowForNonTerminal(rule, symbolIndex, changed);
	}
}

void FollowSetCalculator::ProcessFollowForNonTerminal(
	const Production& rule,
	const size_t symbolIndex,
	bool& changed)
{
	auto& targetFollow = m_follow[rule.GetRhs()[symbolIndex]];
	const size_t sizeBefore = targetFollow.size();

	const std::span<const Symbol> tail = GetSymbolsAfter(rule.GetRhs(), symbolIndex);
	const FirstSetResolver resolver(m_nullable, m_first);
	const std::set<Symbol> tailFirst = resolver.GetFirstOfSequence(tail);

	AddFirstOfTailToFollow(targetFollow, tailFirst);
	AddLhsFollowIfTailNullable(targetFollow, rule.GetLhs(), tailFirst, m_follow);

	if (targetFollow.size() > sizeBefore)
	{
		changed = true;
	}
}

std::span<const Symbol> FollowSetCalculator::GetSymbolsAfter(
	const std::vector<Symbol>& rhs,
	const size_t symbolIndex)
{
	return std::span<const Symbol>(rhs).subspan(symbolIndex + 1);
}

void FollowSetCalculator::AddFirstOfTailToFollow(
	std::set<Symbol>& targetFollow,
	const std::set<Symbol>& tailFirst)
{
	for (const auto& symbol : tailFirst)
	{
		if (!symbol.IsEpsilon())
		{
			targetFollow.insert(symbol);
		}
	}
}

void FollowSetCalculator::AddLhsFollowIfTailNullable(
	std::set<Symbol>& targetFollow,
	const Symbol& lhs,
	const std::set<Symbol>& tailFirst,
	const FollowSets& follow)
{
	if (!tailFirst.contains(EpsilonSymbol()))
	{
		return;
	}

	const auto lhsFollowIt = follow.find(lhs);
	if (lhsFollowIt == follow.end())
	{
		return;
	}

	targetFollow.insert(lhsFollowIt->second.begin(), lhsFollowIt->second.end());
}
