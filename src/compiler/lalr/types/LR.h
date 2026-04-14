#pragma once
#include "src/common/grammar/Symbol.h"

struct LR0Item
{
	int ruleIndex{};
	size_t dotPos{};
	auto operator<=>(const LR0Item&) const = default;
};

struct LR1Item
{
	LR0Item core;
	Symbol lookahead;
	auto operator<=>(const LR1Item&) const = default;
};
