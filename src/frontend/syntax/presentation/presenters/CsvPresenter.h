#pragma once

#include "src/frontend/syntax/grammar/Grammar.h"
#include "src/frontend/syntax/grammar/Symbol.h"
#include "src/frontend/syntax/lalr/types/LALR.h"
#include "src/frontend/syntax/presentation/ParseTablePresenter.h"

#include <ostream>
#include <string>
#include <vector>

class CsvPresenter final : public ParseTablePresenter
{
public:
	explicit CsvPresenter(std::string filePath);

	void Present(const ParseTable& table, const Grammar& grammar) const override;

private:
	static std::vector<Symbol> GetAllSymbols(const Grammar& grammar);
	static void WriteHeader(std::ostream& outputStream, const std::vector<Symbol>& allSymbols);
	static void WriteRows(std::ostream& outputStream, const ParseTable& table, const std::vector<Symbol>& allSymbols);

	std::string m_filePath;
};
