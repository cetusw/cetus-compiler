#pragma once
#include "types/value.h"
#include <iostream>

namespace NativeFunctions
{
inline Value NativePrint(int argc, Value* args)
{
	for (int i = 0; i < argc; i++)
	{
		args[i].Print();
	}
	return {};
}

inline Value NativePrintln(int argc, Value* args)
{
	NativePrint(argc, args);
	std::printf("\n");
	return {};
}

inline Value NativeLen(int argc, Value* args)
{
	if (argc != 1 || !args[0].IsString())
	{
		return {};
	}
	return Value(static_cast<double>(args[0].AsString().length()));
}
} // namespace NativeFunctions
