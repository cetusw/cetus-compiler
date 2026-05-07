#pragma once

#include "src/backend/vm/objects/ObjFunction.h"
#include <memory>
#include <optional>
#include <string>

struct CodegenResult
{
	[[nodiscard]] static CodegenResult Success(std::shared_ptr<ObjFunction> function)
	{
		CodegenResult result;
		result.function = std::move(function);
		return result;
	}

	[[nodiscard]] static CodegenResult Error(std::string message)
	{
		CodegenResult result;
		result.error = std::move(message);
		return result;
	}

	std::shared_ptr<ObjFunction> function;
	std::optional<std::string> error;
};
