#include "NativeRegistry.h"

#include "NativeFunctions.h"
#include "objects/ObjNative.h"
#include "vm.h"

void NativeRegistry::RegisterStdLib(VM& vm)
{
	vm.DefineGlobal("print", Value(std::make_shared<ObjNative>(NativeFunctions::NativePrint)));
	vm.DefineGlobal("println", Value(std::make_shared<ObjNative>(NativeFunctions::NativePrintln)));
	vm.DefineGlobal("len", Value(std::make_shared<ObjNative>(NativeFunctions::NativeLen)));
	vm.DefineGlobal("scan", Value(std::make_shared<ObjNative>(NativeFunctions::NativeScan)));
}
