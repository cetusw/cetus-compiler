#pragma once

#include "lalr/types/PreparedGrammar.h"
#include <string>

class GrammarPreparator
{
public:
	explicit GrammarPreparator(
		std::string grammarPath = "docs/cetus_grammar.txt",
		std::string preparedGrammarPath = "docs/prepared_grammar.json");

	[[nodiscard]] PreparedGrammar Prepare(bool regenerateTable) const;

private:
	std::string m_grammarPath;
	std::string m_preparedGrammarPath;
};
