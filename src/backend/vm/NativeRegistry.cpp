#include "NativeRegistry.h"

#include "NativeFunctions.h"
#include "objects/ObjNative.h"
#include "vm.h"

void NativeRegistry::RegisterStdLib(VM& vm)
{
	vm.DefineGlobal("print", Value(std::make_shared<ObjNative>(NativeFunctions::NativePrint)));
	vm.DefineGlobal("println", Value(std::make_shared<ObjNative>(NativeFunctions::NativePrintln)));
	vm.DefineGlobal("len", Value(std::make_shared<ObjNative>(NativeFunctions::NativeLen)));
	vm.DefineGlobal("readString", Value(std::make_shared<ObjNative>(NativeFunctions::NativeReadString)));
	vm.DefineGlobal("readInt", Value(std::make_shared<ObjNative>(NativeFunctions::NativeReadInt)));
	vm.DefineGlobal("readFloat", Value(std::make_shared<ObjNative>(NativeFunctions::NativeReadFloat)));
	vm.DefineGlobal("readBool", Value(std::make_shared<ObjNative>(NativeFunctions::NativeReadBool)));
}
