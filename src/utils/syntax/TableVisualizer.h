#pragma once
#include "src/frontend/syntax/grammar/Grammar.h"
#include "src/frontend/syntax/lalr/types/LALR.h"
#include <map>
#include <string>
#include <vector>

class TableVisualizer
{
public:
	static void PrintTable(const ParseTable& table, const Grammar& grammar);
	static std::string ActionToString(const Action& action);

private:
	static std::vector<Symbol> GetOrderedColumns(const Grammar& grammar);
	static std::map<Symbol, int> CalculateColumnWidths(const ParseTable& table, const std::vector<Symbol>& cols);
	static int GetStateColumnWidth(const ParseTable& table);

	static void PrintSeparator(int stateWidth, const std::vector<Symbol>& cols, const std::map<Symbol, int>& widths);
	static void PrintHeader(int stateWidth, const std::vector<Symbol>& cols, const std::map<Symbol, int>& widths);
	static void PrintRows(const ParseTable& table, int stateWidth, const std::vector<Symbol>& cols, const std::map<Symbol, int>& widths);

	static int GetVisibleLength(const std::string& text);
	static std::string CenterText(const std::string& text, int width);
};
