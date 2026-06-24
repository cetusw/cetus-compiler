#pragma once

#include "../../syntax/table/Action.h"
#include "LR.h"
#include <map>
#include <set>

using LR0StateCore = std::set<LR0ItemCore>;
using LR1ItemSet = std::set<LR1Item>;
using LALRState = std::set<LR1Item>;