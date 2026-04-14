#pragma once
#include "src/common/grammar/Grammar.h"
#include "src/compiler/lalr/types/LALR.h"

class TableVisualizer
{
public:
	static void PrintTable(const ParseTable& table, const Grammar& grammar);

private:
	static std::string ActionToString(const Action& action);
};