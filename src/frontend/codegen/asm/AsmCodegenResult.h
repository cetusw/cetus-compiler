#pragma once

#include <optional>
#include <string>

struct AsmCodegenResult
{
	[[nodiscard]] static AsmCodegenResult Success(std::string text)
	{
		AsmCodegenResult result;
		result.text = std::move(text);
		return result;
	}

	[[nodiscard]] static AsmCodegenResult Error(std::string message)
	{
		AsmCodegenResult result;
		result.error = std::move(message);
		return result;
	}

	std::string text;
	std::optional<std::string> error;
};
