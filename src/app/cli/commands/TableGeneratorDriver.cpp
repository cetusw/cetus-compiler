#include "TableGeneratorDriver.h"
#include "src/frontend/syntax/grammar/GrammarLoader.h"
#include "src/frontend/syntax/lalr/LALRBuilder.h"
#include "src/utils/syntax/TableExporter.h"
#include "src/utils/syntax/TableVisualizer.h"
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
	const ParserDefinition definition = builder.Build();

	TableVisualizer::PrintTable(definition.table, grammar);
	TableExporter::ExportToCsv(definition.table, grammar, configuration.outputFilePath);
}
