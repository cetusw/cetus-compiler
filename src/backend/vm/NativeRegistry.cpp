#include "NativeRegistry.h"

#include "NativeFunctions.h"
#include "objects/ObjNative.h"
#include "vm.h"

void NativeRegistry::RegisterStdLib(VM& vm)
{
	vm.DefineGlobal("print", Value(std::make_shared<ObjNative>(NativeFunctions::NativePrint, 0)));
	vm.DefineGlobal("println", Value(std::make_shared<ObjNative>(NativeFunctions::NativePrintln, 0)));
	vm.DefineGlobal("append", Value(std::make_shared<ObjNative>(NativeFunctions::NativeAppend, 1)));
	vm.DefineGlobal("len", Value(std::make_shared<ObjNative>(NativeFunctions::NativeLen, 1)));
	vm.DefineGlobal("scan", Value(std::make_shared<ObjNative>(NativeFunctions::NativeScan, 0)));
}
