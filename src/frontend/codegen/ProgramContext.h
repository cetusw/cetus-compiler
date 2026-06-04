#pragma once

#include "src/backend/vm/objects/ObjFunction.h"
#include "src/backend/vm/types/Program.h"
#include <memory>
#include <vector>

class ProgramContext
{
public:
	void Reset();
	void AddFunction(std::shared_ptr<ObjFunction> function);
	[[nodiscard]] bool HasEntryPoint() const;
	[[nodiscard]] Program Build();

private:
	std::vector<std::shared_ptr<ObjFunction>> m_functions;
	std::shared_ptr<ObjFunction> m_entryPoint;
};
