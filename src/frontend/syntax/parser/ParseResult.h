#pragma once

#include "src/frontend/syntax/ast/ASTNode.h"
#include <string>
#include <utility>
#include <vector>

struct ParseResult
{
	[[nodiscard]] static ParseResult Success(const int line, ASTNodePtr ast = nullptr, std::string message = "Syntax analysis completed successfully.")
	{
		ParseResult result;
		result.success = true;
		result.line = line;
		result.message = std::move(message);
		result.ast = std::move(ast);
		return result;
	}

	[[nodiscard]] static ParseResult Error(const int line, std::string message)
	{
		ParseResult result;
		result.line = line;
		result.message = std::move(message);
		return result;
	}

	[[nodiscard]] static ParseResult Error(const int line, std::string message, std::vector<std::string> expectedTerminals)
	{
		ParseResult result = Error(line, std::move(message));
		result.expectedTerminals = std::move(expectedTerminals);
		return result;
	}

	bool success = false;
	int line = 1;
	std::string message;
	std::vector<std::string> expectedTerminals;
	ASTNodePtr ast;
};
