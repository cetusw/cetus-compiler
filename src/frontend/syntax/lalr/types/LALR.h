#pragma once
#include "Action.h"
#include "LR.h"
#include <map>
#include <set>

using LALRState = std::set<LR1Item>;
using ParseTable = std::map<int, std::map<Symbol, Action>>;
