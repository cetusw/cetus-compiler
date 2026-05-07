#include "PredefinedSymbols.h"

#include "src/backend/vm/vm.h"

SymbolTable PredefinedSymbols::CreateSymbolTable()
{
	return SymbolTable({
		{"a", Symbol{"a", Type::INT}},
		{"b", Symbol{"b", Type::FLOAT}},
		{"x", Symbol{"x", Type::INT}},
		{"flag", Symbol{"flag", Type::BOOL}}
	});
}

void PredefinedSymbols::LoadRuntimeGlobals(VM& vm)
{
	vm.DefineGlobal("a", Value(10.0));
	vm.DefineGlobal("b", Value(2.5));
	vm.DefineGlobal("x", Value(7.0));
	vm.DefineGlobal("flag", Value(true));
}
