#pragma once

#include "src/frontend/syntax/semantic/SymbolTable.h"

class VM;

// TODO удалить как только будет присвоение
class PredefinedSymbols
{
public:
	[[nodiscard]] static SymbolTable CreateSymbolTable();
	static void LoadRuntimeGlobals(VM& vm);
};
