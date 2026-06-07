#pragma once

#include "BytecodeEmitter.h"
#include "src/backend/vm/objects/ObjFunction.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class FunctionContext
{
public:
	FunctionContext(std::shared_ptr<ObjFunction> function, std::optional<std::string>& error);

	[[nodiscard]] std::shared_ptr<ObjFunction> GetFunction() const;
	[[nodiscard]] ObjFunction& Function() const;
	[[nodiscard]] BytecodeEmitter& Emitter();
	void RegisterParameter(const std::string& name, int slot);
	void BeginScope();
	[[nodiscard]] int EndScope();
	[[nodiscard]] int CurrentScopeDepth() const;
	int DeclareLocal(const std::string& name);
	[[nodiscard]] std::optional<int> ResolveLocal(const std::string& name) const;
	[[nodiscard]] int CountLocalsAboveDepth(int scopeDepth) const;

private:
	struct LocalSlot
	{
		std::string name;
		int slot;
		int scopeDepth;
	};

	std::shared_ptr<ObjFunction> m_function;
	BytecodeEmitter m_emitter;
	std::vector<LocalSlot> m_localSlots;
	int m_scopeDepth = 0;
	int m_nextLocalSlot = 1;
};
