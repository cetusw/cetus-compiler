#include "ParserDriver.h"

#include "src/compiler/lalr/LALRBuilder.h"
#include "src/compiler/lexer/Lexer.h"
#include "src/compiler/parser/TableDrivenParser.h"
#include "src/utils/io/GrammarLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace
{
std::string ReadFile(const std::string& path)
{
	std::ifstream input(path);
	if (!input.is_open())
	{
		throw std::runtime_error("Failed to open source file: " + path);
	}

	std::ostringstream content;
	content << input.rdbuf();
	return content.str();
}
} // namespace

void ParserDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input source file path is required for parsing.");
	}

	// TODO решить, как хранить грамматику
	const std::string source = ReadFile(configuration.inputFilePath);

	Lexer lexer(source);
	const std::vector<Token> tokens = lexer.ScanTokens();

	CheckIfError(tokens);

	const Grammar grammar = GetGrammar();
	LALRBuilder builder(grammar);
	ParserDefinition definition = builder.Build();
	TableDrivenParser parser(definition);

	const ParseResult result = parser.Parse(tokens);
	if (!result.success)
	{
		throw std::runtime_error(result.message);
	}

	std::cout << result.message << std::endl;
}

// TODO подумать над названием
void ParserDriver::CheckIfError(const std::vector<Token>& tokens)
{
	for (const Token& token : tokens)
	{
		if (token.type == TokenType::ERROR)
		{
			throw std::runtime_error("Lexical error at line " + std::to_string(token.line) + ": " + token.lexeme);
		}
	}
}

Grammar ParserDriver::GetGrammar()
{
	constexpr auto grammarPath = "docs/cetus_grammar.txt";
	return GrammarLoader::LoadFromFile(grammarPath);
}
