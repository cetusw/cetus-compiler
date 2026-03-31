#include "TableGeneratorDriver.h"
#include "../../compiler/slr-builder/AutomatonBuilder.h"
#include "../../utils/io/GrammarLoader.h"
#include "../../utils/io/TableVisualizer.h"
#include "src/utils/io/TableExporter.h"

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

	AutomatonBuilder builder(grammar);
	const SLRTable table = builder.Build();

	TableVisualizer::PrintTable(table, grammar);
	TableExporter::ExportToCsv(table, grammar, configuration.outputFilePath);
}