#pragma once

#include "objects/ObjString.h"
#include "types/Value.h"
#include <iostream>
#include <string>

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
	return Value(static_cast<RuntimeInt>(args[0].AsString().length()));
}

inline Value NativeReadString(int argc, Value* args)
{
	(void)args;
	if (argc != 0)
	{
		return {};
	}

	std::string value;
	if (!(std::cin >> value))
	{
		std::cin.clear();
		value.clear();
	}
	return Value(std::make_shared<ObjString>(std::move(value)));
}

inline Value NativeReadInt(int argc, Value* args)
{
	(void)args;
	if (argc != 0)
	{
		return {};
	}

	RuntimeInt value = 0;
	if (!(std::cin >> value))
	{
		std::cin.clear();
		return Value(static_cast<RuntimeInt>(0));
	}
	return Value(value);
}

inline Value NativeReadFloat(int argc, Value* args)
{
	(void)args;
	if (argc != 0)
	{
		return {};
	}

	RuntimeFloat value = 0.0;
	if (!(std::cin >> value))
	{
		std::cin.clear();
		return Value(0.0);
	}
	return Value(value);
}

inline Value NativeReadBool(int argc, Value* args)
{
	(void)args;
	if (argc != 0)
	{
		return {};
	}

	std::string value;
	if (!(std::cin >> value))
	{
		std::cin.clear();
		return Value(false);
	}
	return Value(value == "true" || value == "1");
}
} // namespace NativeFunctions
