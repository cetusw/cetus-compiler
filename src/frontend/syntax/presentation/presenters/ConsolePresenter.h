#pragma once
#include "../ParseTablePresenter.h"
#include "src/frontend/syntax/grammar/Grammar.h"
#include "src/frontend/syntax/lalr/types/LALR.h"
#include <map>
#include <string>
#include <vector>

class ConsolePresenter final : public ParseTablePresenter
{
public:
	void Present(const ParseTable& table, const Grammar& grammar) const override;

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
