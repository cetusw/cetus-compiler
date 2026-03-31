#include "TableVisualizer.h"
#include "../../vm/common.h"
#include <iomanip>
#include <iostream>

void TableVisualizer::PrintTable(const SLRTable& table, const Grammar& grammar)
{
	if constexpr (!DEBUG_MODE)
	{
		std::cout << "SLR Table generated successfully." << std::endl;
		return;
	}

	std::set<Symbol> allSymbols = grammar.GetTerminals();
	auto nonTerminals = grammar.GetNonTerminals();
	allSymbols.insert(nonTerminals.begin(), nonTerminals.end());
	allSymbols.insert({ "⊥", true });

	std::cout << std::setw(8) << "State" << " | ";
	for (const auto& symbol : allSymbols)
	{
		std::cout << std::setw(10) << symbol.GetValue() << " | ";
	}
	std::cout << "\n"
			  << std::string(120, '-') << "\n";

	for (const auto& [stateId, row] : table)
	{
		std::cout << std::setw(8) << stateId << " | ";
		for (const auto& symbol : allSymbols)
		{
			std::string cell = "-";
			if (row.contains(symbol))
			{
				cell = ActionToString(row.at(symbol));
			}
			std::cout << std::setw(10) << cell << " | ";
		}
		std::cout << "\n";
	}
}

std::string TableVisualizer::ActionToString(const Action& action)
{
	switch (action.type)
	{
	case ActionType::SHIFT:
		return "s" + std::to_string(action.value);
	case ActionType::REDUCE:
		return "r" + std::to_string(action.value);
	case ActionType::GOTO:
		return "g" + std::to_string(action.value);
	case ActionType::ACCEPT:
		return "ACC";
	default:
		return "-";
	}
}