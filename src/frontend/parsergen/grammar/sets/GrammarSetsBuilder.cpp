#include "GrammarSetsBuilder.h"

#include "calculators/FirstSetCalculator.h"
#include "calculators/FollowSetCalculator.h"
#include "calculators/NullableSetCalculator.h"

GrammarSets GrammarSetsBuilder::Build(const Grammar& grammar)
{
	NullableSet nullable = NullableSetCalculator::Calculate(grammar);
	FirstSets first = FirstSetCalculator::Calculate(grammar, nullable);
	FollowSets follow = FollowSetCalculator::Calculate(grammar, nullable, first);

	return {
		.nullable = std::move(nullable),
		.first = std::move(first),
		.follow = std::move(follow)
	};
}
