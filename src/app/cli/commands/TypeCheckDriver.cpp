#include "TypeCheckDriver.h"

#include "src/frontend/syntax/SyntaxAnalyzer.h"
#include "src/frontend/syntax/semantic/TypeCheckerVisitor.h"
#include "src/frontend/syntax/semantic/TypeRules.h"
#include <iostream>
#include <stdexcept>

namespace
{
// TODO костыль
TypeEnvironment CreateEnvironment()
{
	return TypeEnvironment({
		{"a", Type::INT},
		{"b", Type::FLOAT},
		{"x", Type::INT},
		{"flag", Type::BOOL}
	});
}
}

void TypeCheckDriver::Execute(const Configuration& configuration)
{
	if (configuration.inputFilePath.empty())
	{
		throw std::runtime_error("Input source file path is required for type checking.");
	}

	const SyntaxAnalyzer analyzer;
	const ParseResult parseResult = analyzer.ParseFile(configuration.inputFilePath);
	if (!parseResult.success)
	{
		throw std::runtime_error(parseResult.message);
	}
	if (!parseResult.ast)
	{
		throw std::runtime_error("AST was not produced for parsed input.");
	}

	TypeCheckerVisitor checker(CreateEnvironment());
	const TypeCheckResult typeResult = checker.Check(*parseResult.ast);
	if (typeResult.error.has_value())
	{
		throw std::runtime_error(*typeResult.error);
	}

	std::cout << "Type: " << TypeRules::ToString(typeResult.type) << std::endl;
}
