#include "NativeRegistry.h"

#include "NativeFunctions.h"
#include "objects/ObjNative.h"
#include "vm.h"

void NativeRegistry::RegisterStdLib(VM& vm)
{
	vm.RegisterImmutableGlobal("print", Value(std::make_shared<ObjNative>(NativeFunctions::NativePrint, 0)));
	vm.RegisterImmutableGlobal("println", Value(std::make_shared<ObjNative>(NativeFunctions::NativePrintln, 0)));
	vm.RegisterImmutableGlobal("append", Value(std::make_shared<ObjNative>(NativeFunctions::NativeAppend, 1)));
	vm.RegisterImmutableGlobal("len", Value(std::make_shared<ObjNative>(NativeFunctions::NativeLen, 1)));
	vm.RegisterImmutableGlobal("scan", Value(std::make_shared<ObjNative>(NativeFunctions::NativeScan, 0)));
}
