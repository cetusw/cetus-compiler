#pragma once

#include "BytecodeEmitter.h"
#include "src/backend/vm/objects/ObjFunction.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

class FunctionContext
{
public:
	FunctionContext(std::shared_ptr<ObjFunction> function, std::optional<std::string>& error);

	[[nodiscard]] std::shared_ptr<ObjFunction> GetFunction() const;
	[[nodiscard]] ObjFunction& Function() const;
	[[nodiscard]] BytecodeEmitter& Emitter();
	void RegisterParameter(const std::string& name, int slot);
	[[nodiscard]] std::optional<int> ResolveLocal(const std::string& name) const;

private:
	std::shared_ptr<ObjFunction> m_function;
	BytecodeEmitter m_emitter;
	std::unordered_map<std::string, int> m_localSlots;
};
