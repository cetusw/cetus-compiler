#pragma once

#include "Action.h"
#include "Symbol.h"

#include <map>

using ParseTable = std::map<int, std::map<Symbol, Action>>;
