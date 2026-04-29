#include "TableExporter.h"
#include <fstream>

void TableExporter::ExportToCsv(const ParseTable& table, const Grammar& grammar, const std::string& filePath)
{
	std::ofstream fileStream(filePath);

	if (!fileStream.is_open())
	{
		throw std::runtime_error("Could not open file for export: " + filePath);
	}

	const std::vector<Symbol> allSymbols = GetAllSymbols(grammar);

	WriteHeader(fileStream, allSymbols);
	WriteRows(fileStream, table, allSymbols);

	fileStream.close();
}

std::vector<Symbol> TableExporter::GetAllSymbols(const Grammar& grammar)
{
	std::vector<Symbol> allSymbols;

	const std::set<Symbol> terminals = grammar.GetTerminals();
	allSymbols.insert(allSymbols.end(), terminals.begin(), terminals.end());

	const std::set<Symbol> nonTerminals = grammar.GetNonTerminals();
	allSymbols.insert(allSymbols.end(), nonTerminals.begin(), nonTerminals.end());

	allSymbols.emplace_back("⊥", true);

	return allSymbols;
}

void TableExporter::WriteHeader(std::ostream& outputStream, const std::vector<Symbol>& allSymbols)
{
	outputStream << "\"State\"";
	for (const auto& symbol : allSymbols)
	{
		outputStream << ",\"" << symbol.GetValue() << "\"";
	}
	outputStream << "\n";
}

void TableExporter::WriteRows(std::ostream& outputStream,
	const ParseTable& table,
	const std::vector<Symbol>& allSymbols)
{
	for (const auto& [stateIdentifier, rowMap] : table)
	{
		outputStream << stateIdentifier;

		for (const auto& symbol : allSymbols)
		{
			outputStream << ",";
			if (rowMap.contains(symbol))
			{
				outputStream << "\"" << ActionToString(rowMap.at(symbol)) << "\"";
			}
			else
			{
				outputStream << "\"\"";
			}
		}
		outputStream << "\n";
	}
}

std::string TableExporter::ActionToString(const Action& action)
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
		return "";
	}
}