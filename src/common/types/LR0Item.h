#pragma once
#include <compare>
#include <cstddef>

struct LR0Item {
	int ruleIndex{};
	size_t dotPos{};
	auto operator<=>(const LR0Item&) const = default;
};
