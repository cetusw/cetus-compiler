#include "CsvPresenter.h"

#include "src/frontend/syntax/lalr/ActionFormatter.h"
#include "src/support/io/FileReader.h"

#include <sstream>
#include <stdexcept>
#include <utility>

CsvPresenter::CsvPresenter(std::string filePath)
	: m_filePath(std::move(filePath))
{
}

void CsvPresenter::Present(const ParseTable& table, const Grammar& grammar) const
{
	std::ostringstream outputStream;

	const std::vector<Symbol> allSymbols = GetAllSymbols(grammar);

	WriteHeader(outputStream, allSymbols);
	WriteRows(outputStream, table, allSymbols);
	FileReader::WriteAll(m_filePath, outputStream.str());
}

std::vector<Symbol> CsvPresenter::GetAllSymbols(const Grammar& grammar)
{
	std::vector<Symbol> allSymbols;

	const std::set<Symbol> terminals = grammar.GetTerminals();
	allSymbols.insert(allSymbols.end(), terminals.begin(), terminals.end());

	const std::set<Symbol> nonTerminals = grammar.GetNonTerminals();
	allSymbols.insert(allSymbols.end(), nonTerminals.begin(), nonTerminals.end());

	allSymbols.emplace_back("⊥", true);

	return allSymbols;
}

void CsvPresenter::WriteHeader(std::ostream& outputStream, const std::vector<Symbol>& allSymbols)
{
	outputStream << "\"State\"";
	for (const auto& symbol : allSymbols)
	{
		outputStream << ",\"" << symbol.GetValue() << "\"";
	}
	outputStream << "\n";
}

void CsvPresenter::WriteRows(std::ostream& outputStream,
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
				outputStream << "\"" << ActionFormatter::Format(rowMap.at(symbol), ActionFormat::Compact) << "\"";
			}
			else
			{
				outputStream << "\"\"";
			}
		}
		outputStream << "\n";
	}
}
