#include "TableGeneratorDriver.h"

#include "../../../frontend/syntax/presentation/presenters/ConsolePresenter.h"
#include "src/frontend/syntax/grammar/GrammarLoader.h"
#include "src/frontend/syntax/lalr/LALRBuilder.h"
#include "src/frontend/syntax/presentation/presenters/CsvPresenter.h"

#include <iostream>
#include <stdexcept>

void TableGeneratorDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input grammar file path is required for table generation.");
	}

	if (configuration.outputFilePath.empty())
	{
		throw std::runtime_error("Output file path is required for table generation.");
	}

	if (configuration.verbose)
	{
		std::cout << "Loading grammar from: " << configuration.inputFilePath << std::endl;
	}

	const Grammar grammar = GrammarLoader::LoadFromFile(configuration.inputFilePath);

	LALRBuilder builder(grammar);
	const PreparedGrammar preparedGrammar = builder.Build();

	const ConsolePresenter presenter;
	presenter.Present(preparedGrammar.table, grammar);

	const CsvPresenter exporter(configuration.outputFilePath);
	exporter.Present(preparedGrammar.table, grammar);
}
