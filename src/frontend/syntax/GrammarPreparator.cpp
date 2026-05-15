#include "GrammarPreparator.h"
#include "grammar/GrammarLoader.h"
#include "lalr/LALRBuilder.h"
#include "storage/JsonPreparedGrammarStorage.h"
#include <utility>

GrammarPreparator::GrammarPreparator(std::string grammarPath, std::string preparedGrammarPath)
	: m_grammarPath(std::move(grammarPath))
	, m_preparedGrammarPath(std::move(preparedGrammarPath))
{
}

PreparedGrammar GrammarPreparator::Prepare(const bool regenerateTable) const
{
	const JsonPreparedGrammarStorage storage;
	if (!regenerateTable)
	{
		return storage.Load(m_preparedGrammarPath);
	}

	const Grammar grammar = GrammarLoader::LoadFromFile(m_grammarPath);
	LALRBuilder builder(grammar);
	PreparedGrammar preparedGrammar = builder.Build();
	storage.Save(preparedGrammar, m_preparedGrammarPath);
	return preparedGrammar;
}
