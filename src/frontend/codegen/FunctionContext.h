#pragma once

#include "BytecodeEmitter.h"
#include "src/backend/vm/objects/ObjFunction.h"
#include <memory>
#include <optional>
#include <string>

class FunctionContext
{
public:
	FunctionContext(std::shared_ptr<ObjFunction> function, std::optional<std::string>& error);

	[[nodiscard]] std::shared_ptr<ObjFunction> GetFunction() const;
	[[nodiscard]] ObjFunction& Function() const;
	[[nodiscard]] BytecodeEmitter& Emitter();

private:
	std::shared_ptr<ObjFunction> m_function;
	BytecodeEmitter m_emitter;
};
