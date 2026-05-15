#include "PredefinedSymbols.h"

#include "src/backend/vm/vm.h"

SymbolTable PredefinedSymbols::CreateSymbolTable()
{
	return SymbolTable({
		{"a", SemanticSymbol{"a", Type::INT}},
		{"b", SemanticSymbol{"b", Type::FLOAT}},
		{"x", SemanticSymbol{"x", Type::INT}},
		{"flag", SemanticSymbol{"flag", Type::BOOL}}
	});
}

void PredefinedSymbols::LoadRuntimeGlobals(VM& vm)
{
	vm.DefineGlobal("a", Value(10.0));
	vm.DefineGlobal("b", Value(2.5));
	vm.DefineGlobal("x", Value(7.0));
	vm.DefineGlobal("flag", Value(true));
}
