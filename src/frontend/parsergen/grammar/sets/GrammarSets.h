#pragma once

#include "src/frontend/syntax/table/Symbol.h"

#include <map>
#include <set>

using NullableSet = std::set<Symbol>;
using FirstSets = std::map<Symbol, std::set<Symbol>>;
using FollowSets = std::map<Symbol, std::set<Symbol>>;

struct GrammarSets
{
	NullableSet nullable;
	FirstSets first;
	FollowSets follow;
};
