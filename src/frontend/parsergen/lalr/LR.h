#pragma once
#include "src/frontend/syntax/table/Symbol.h"

struct LR0ItemCore
{
	int productionIndex{};
	size_t dotPosition{};
	auto operator<=>(const LR0ItemCore&) const = default;
};

struct LR1Item
{
	LR0ItemCore core;
	Symbol lookahead;
	auto operator<=>(const LR1Item&) const = default;
};
