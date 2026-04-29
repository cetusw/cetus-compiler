#pragma once
#include "src/frontend/syntax/grammar/Grammar.h"
#include "src/frontend/syntax/lalr/types/LALR.h"
#include <string>
#include <vector>

class TableExporter
{
public:
	static void ExportToCsv(const ParseTable& table, const Grammar& grammar, const std::string& filePath);

private:
	static std::string ActionToString(const Action& action);
	static std::vector<Symbol> GetAllSymbols(const Grammar& grammar);
	static void WriteHeader(std::ostream& outputStream, const std::vector<Symbol>& allSymbols);
	static void WriteRows(std::ostream& outputStream, const ParseTable& table, const std::vector<Symbol>& allSymbols);
};
