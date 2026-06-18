#pragma once

#include "src/app/diagnostics/DiagnosticStage.h"
#include "src/frontend/syntax/ast/ASTNode.h"
#include <string>
#include <utility>
#include <vector>

struct ParseResult
{
	[[nodiscard]] static ParseResult Success(
		const int line,
		std::unique_ptr<ProgramASTNode> ast,
		std::string message = "Syntax analysis completed successfully.")
	{
		ParseResult result;
		result.success = true;
		result.stage = DiagnosticStage::Syntax;
		result.line = line;
		result.message = std::move(message);
		result.ast = std::move(ast);
		return result;
	}

	[[nodiscard]] static ParseResult Error(
		const DiagnosticStage stage,
		const int line,
		std::string message)
	{
		ParseResult result;
		result.stage = stage;
		result.line = line;
		result.message = std::move(message);
		return result;
	}

	[[nodiscard]] static ParseResult Error(const int line, std::string message)
	{
		return Error(DiagnosticStage::Syntax, line, std::move(message));
	}

	[[nodiscard]] static ParseResult Error(
		const DiagnosticStage stage,
		const int line,
		std::string message,
		std::vector<std::string> expectedTerminals)
	{
		ParseResult result = Error(stage, line, std::move(message));
		result.expectedTerminals = std::move(expectedTerminals);
		return result;
	}

	[[nodiscard]] static ParseResult Error(
		const int line,
		std::string message,
		std::vector<std::string> expectedTerminals)
	{
		return Error(
			DiagnosticStage::Syntax,
			line,
			std::move(message),
			std::move(expectedTerminals));
	}

	bool success = false;
	DiagnosticStage stage = DiagnosticStage::Syntax;
	int line = 1;
	std::string message;
	std::vector<std::string> expectedTerminals;
	std::unique_ptr<ProgramASTNode> ast;
};
