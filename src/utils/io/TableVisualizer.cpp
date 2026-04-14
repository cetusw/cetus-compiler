#include "TableVisualizer.h"
#include "../../vm/common.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <ranges>

void TableVisualizer::PrintTable(const ParseTable& table, const Grammar& grammar)
{
	if constexpr (!DEBUG_MODE)
	{
		std::cout << "Table generated successfully." << std::endl;
		return;
	}

	auto cols = GetOrderedColumns(grammar);
	auto widths = CalculateColumnWidths(table, cols);
	int stateWidth = GetStateColumnWidth(table);

	std::cout << "\nPARSE TABLE\n";
	PrintSeparator(stateWidth, cols, widths);
	PrintHeader(stateWidth, cols, widths);
	PrintSeparator(stateWidth, cols, widths);
	PrintRows(table, stateWidth, cols, widths);
	PrintSeparator(stateWidth, cols, widths);
	std::cout << std::endl;
}

std::vector<Symbol> TableVisualizer::GetOrderedColumns(const Grammar& grammar)
{
	std::vector<Symbol> cols;
	auto nonTerminals = grammar.GetNonTerminals();
	cols.insert(cols.end(), nonTerminals.begin(), nonTerminals.end());

	auto terminals = grammar.GetTerminals();
	cols.insert(cols.end(), terminals.begin(), terminals.end());

	cols.emplace_back("⊥", true);
	return cols;
}

std::map<Symbol, int> TableVisualizer::CalculateColumnWidths(const ParseTable& table, const std::vector<Symbol>& cols)
{
	std::map<Symbol, int> widths;
	for (const auto& sym : cols)
	{
		int maxLen = GetVisibleLength(sym.GetValue());
		for (const auto& row : table | std::views::values)
		{
			if (row.contains(sym))
			{
				int actionLen = static_cast<int>(ActionToString(row.at(sym)).length());
				maxLen = std::max(maxLen, actionLen);
			}
		}
		widths[sym] = maxLen + 2;
	}
	return widths;
}

int TableVisualizer::GetStateColumnWidth(const ParseTable& table)
{
	int maxId = 0;
	for (const auto& id : table | std::views::keys)
	{
		maxId = std::max(maxId, id);
	}
	return std::max(5, static_cast<int>(std::to_string(maxId).length())) + 2;
}

void TableVisualizer::PrintSeparator(const int stateWidth, const std::vector<Symbol>& cols, const std::map<Symbol, int>& widths)
{
	std::cout << "+" << std::string(stateWidth, '-');
	for (const auto& sym : cols)
	{
		std::cout << "+" << std::string(widths.at(sym), '-');
	}
	std::cout << "+\n";
}

void TableVisualizer::PrintHeader(const int stateWidth, const std::vector<Symbol>& cols, const std::map<Symbol, int>& widths)
{
	std::cout << "|" << CenterText("State", stateWidth);
	for (const auto& sym : cols)
	{
		std::cout << "|" << CenterText(sym.GetValue(), widths.at(sym));
	}
	std::cout << "|\n";
}

void TableVisualizer::PrintRows(const ParseTable& table, int stateWidth, const std::vector<Symbol>& cols, const std::map<Symbol, int>& widths)
{
	std::vector<int> sortedIds;
	for (const auto& id : table | std::views::keys)
	{
		sortedIds.push_back(id);
	}
	std::ranges::sort(sortedIds);

	for (int id : sortedIds)
	{
		const auto& row = table.at(id);
		std::cout << "|" << CenterText(std::to_string(id), stateWidth);

		for (const auto& sym : cols)
		{
			std::string val = row.contains(sym) ? ActionToString(row.at(sym)) : "-";
			std::cout << "|" << CenterText(val, widths.at(sym));
		}
		std::cout << "|\n";
	}
}

int TableVisualizer::GetVisibleLength(const std::string& text)
{
	int length = 0;
	for (size_t i = 0; i < text.length(); ++i)
	{
		if ((static_cast<unsigned char>(text[i]) & 0xC0) != 0x80)
		{
			length++;
		}
	}
	return length;
}

std::string TableVisualizer::CenterText(const std::string& text, int width)
{
	const int visLen = GetVisibleLength(text);
	const int padding = width - visLen;
	if (padding <= 0)
	{
		return text;
	}

	const int left = padding / 2;
	const int right = padding - left;
	return std::string(left, ' ') + text + std::string(right, ' ');
}

std::string TableVisualizer::ActionToString(const Action& action)
{
	switch (action.type)
	{
	case ActionType::SHIFT:
		return "S" + std::to_string(action.value);
	case ActionType::REDUCE:
		return "R" + std::to_string(action.value);
	case ActionType::GOTO:
		return "G" + std::to_string(action.value);
	case ActionType::ACCEPT:
		return "ACC";
	default:
		return "-";
	}
}