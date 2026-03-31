#pragma once

#include "../../compiler/slr-builder/AutomatonBuilder.h"

class TableVisualizer
{
public:
	static void PrintTable(const SLRTable& table, const Grammar& grammar);

private:
	static std::string ActionToString(const Action& action);
};