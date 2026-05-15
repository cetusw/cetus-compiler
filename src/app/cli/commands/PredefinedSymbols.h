#pragma once

#include "../../../frontend/semantic/symbols/SymbolTable.h"

class VM;

// TODO удалить как только будет присвоение
class PredefinedSymbols
{
public:
	[[nodiscard]] static SymbolTable CreateSymbolTable();
	static void LoadRuntimeGlobals(VM& vm);
};
