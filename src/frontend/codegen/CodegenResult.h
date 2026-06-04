#pragma once

#include "src/backend/vm/types/Program.h"
#include <optional>
#include <string>

struct CodegenResult
{
	[[nodiscard]] static CodegenResult Success(Program program)
	{
		CodegenResult result;
		result.program = std::move(program);
		return result;
	}

	[[nodiscard]] static CodegenResult Error(std::string message)
	{
		CodegenResult result;
		result.error = std::move(message);
		return result;
	}

	Program program;
	std::optional<std::string> error;
};
