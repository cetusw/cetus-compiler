#pragma once

#include "../Type.h"
#include "../symbols/SymbolTable.h"
#include <optional>
#include <string>
#include <vector>

struct SemanticDiagnostic
{
	std::string message;
	int line = 0;
};

struct TypeCheckResult
{
	[[nodiscard]] static TypeCheckResult Success(const Type rootType, SymbolTable symbols)
	{
		TypeCheckResult result;
		result.success = true;
		result.type = rootType;
		result.symbols = std::move(symbols);
		return result;
	}

	[[nodiscard]] static TypeCheckResult Success(const Type rootType)
	{
		TypeCheckResult result;
		result.success = true;
		result.type = rootType;
		return result;
	}

	[[nodiscard]] static TypeCheckResult Error(std::string message, SymbolTable symbols)
	{
		TypeCheckResult result;
		result.success = false;
		result.type = Type::ERROR;
		result.symbols = std::move(symbols);
		result.diagnostics.push_back({ std::move(message), 0 });
		return result;
	}

	[[nodiscard]] static TypeCheckResult Error(std::vector<SemanticDiagnostic> diagnostics, SymbolTable symbols)
	{
		TypeCheckResult result;
		result.success = false;
		result.type = Type::ERROR;
		result.symbols = std::move(symbols);
		result.diagnostics = std::move(diagnostics);
		return result;
	}

	[[nodiscard]] static TypeCheckResult Error(std::string message)
	{
		TypeCheckResult result;
		result.success = false;
		result.type = Type::ERROR;
		result.diagnostics.push_back({ std::move(message), 0 });
		return result;
	}

	[[nodiscard]] std::optional<std::string> GetFirstError() const
	{
		if (diagnostics.empty())
		{
			return std::nullopt;
		}
		return diagnostics.front().message;
	}

	[[nodiscard]] std::optional<std::string> GetErrorMessage() const
	{
		if (diagnostics.empty())
		{
			return std::nullopt;
		}

		std::string message;
		for (std::size_t index = 0; index < diagnostics.size(); ++index)
		{
			if (index > 0)
			{
				message += "\n";
			}
			message += diagnostics[index].message;
		}
		return message;
	}

	bool success = false;
	Type type = Type::ERROR;
	SymbolTable symbols;
	std::vector<SemanticDiagnostic> diagnostics;
};
