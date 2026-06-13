#pragma once

#include "objects/ObjArray.h"
#include "objects/ObjRef.h"
#include "objects/ObjSlice.h"
#include "objects/ObjString.h"
#include "types/Value.h"
#include <iostream>
#include <string>

namespace NativeFunctions
{
inline int SequenceLength(const Value& value)
{
	return value.IsArray() ? value.AsArray()->Length() : value.AsSlice()->Length();
}

inline Value NativePrint(int argc, Value* args)
{
	for (int i = 0; i < argc; i++)
	{
		if (i > 0)
		{
			std::printf(" ");
		}
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
	if (argc != 1)
	{
		return {};
	}
	if (args[0].IsNull())
	{
		return Value(static_cast<RuntimeInt>(0));
	}
	if (args[0].IsString())
	{
		return Value(static_cast<RuntimeInt>(args[0].AsString().length()));
	}
	if (args[0].IsSequence())
	{
		return Value(static_cast<RuntimeInt>(SequenceLength(args[0])));
	}
	return {};
}

inline Value NativeAppend(int argc, Value* args)
{
	if (argc < 2)
	{
		return {};
	}

	std::shared_ptr<ObjSlice> slice;
	if (args[0].IsNull())
	{
		auto storage = std::make_shared<ObjArray>(std::vector<Value>{});
		slice = std::make_shared<ObjSlice>(storage, 0, 0, 0);
	}
	else if (args[0].IsSlice())
	{
		slice = args[0].AsSlice();
	}
	else
	{
		return {};
	}

	std::vector<Value> values;
	values.reserve(static_cast<std::size_t>(argc - 1));
	for (int index = 1; index < argc; ++index)
	{
		values.push_back(args[index].Dereference());
	}
	return Value(slice->Append(values));
}

inline Value NativeScan(int argc, Value* args)
{
	if (argc != 1 || !args[0].IsRef())
	{
		return {};
	}

	const auto ref = args[0].AsRef();
	const Value current = ref->Get().Dereference();
	if (current.IsInt())
	{
		RuntimeInt value = 0;
		if (!(std::cin >> value))
		{
			std::cin.clear();
			value = 0;
		}
		ref->Set(Value(value));
		return {};
	}
	if (current.IsFloat())
	{
		RuntimeFloat value = 0.0;
		if (!(std::cin >> value))
		{
			std::cin.clear();
			value = 0.0;
		}
		ref->Set(Value(value));
		return {};
	}
	if (current.IsBool())
	{
		std::string value;
		if (!(std::cin >> value))
		{
			std::cin.clear();
			value.clear();
		}
		ref->Set(Value(value == "true" || value == "1"));
		return {};
	}
	if (current.IsString())
	{
		std::string value;
		if (!(std::cin >> value))
		{
			std::cin.clear();
			value.clear();
		}
		ref->Set(Value(std::make_shared<ObjString>(std::move(value))));
		return {};
	}

	return {};
}
} // namespace NativeFunctions
