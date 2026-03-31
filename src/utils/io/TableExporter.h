#pragma once

#include "../../compiler/slr-builder/AutomatonBuilder.h"
#include <string>
#include <vector>

class TableExporter
{
public:
	static void ExportToCsv(const SLRTable& table, const Grammar& grammar, const std::string& filePath);

private:
	static std::string ActionToString(const Action& action);

	static std::vector<Symbol> GetAllSymbols(const Grammar& grammar);

	static void WriteHeader(std::ostream& outputStream, const std::vector<Symbol>& allSymbols);

	static void WriteRows(std::ostream& outputStream,
		const SLRTable& table,
		const std::vector<Symbol>& allSymbols);
};