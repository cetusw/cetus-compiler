#include "Value.h"
#include "../objects/ObjArray.h"
#include "../objects/ObjFunction.h"
#include "../objects/ObjRef.h"
#include "../objects/ObjString.h"
#include "../objects/ObjSlice.h"
#include "../objects/ObjStruct.h"
#include "src/backend/vm/objects/ObjNative.h"

#include <cassert>
#include <cinttypes>
#include <cstdio>
#include <type_traits>

namespace
{
template <typename TSequence>
void PrintSequence(const TSequence& sequence)
{
	std::printf("[");
	for (int index = 0; index < sequence.Length(); ++index)
	{
		if (index > 0)
		{
			std::printf(", ");
		}
		sequence.Get(index).Print();
	}
	std::printf("]");
}
} // namespace

Value::Value()
	: m_data(nullptr)
{
}

Value::Value(const RuntimeInt value)
	: m_data(value)
{
}

Value::Value(const RuntimeFloat value)
	: m_data(value)
{
}

Value::Value(RuntimeBool value)
	: m_data(value)
{
}

Value::Value(HeapObject obj)
	: m_data(std::move(obj))
{
}

bool Value::IsNull() const
{
	return std::holds_alternative<std::nullptr_t>(m_data);
}

bool Value::IsNumber() const
{
	return IsInt() || IsFloat();
}

bool Value::IsInt() const
{
	return std::holds_alternative<RuntimeInt>(m_data);
}

bool Value::IsFloat() const
{
	return std::holds_alternative<RuntimeFloat>(m_data);
}

bool Value::IsBool() const
{
	return std::holds_alternative<RuntimeBool>(m_data);
}

bool Value::IsString() const
{
	return std::holds_alternative<HeapObject>(m_data) && std::get<HeapObject>(m_data)->GetType() == ObjType::STRING;
}

bool Value::IsFalsey() const
{
	if (IsNull())
	{
		return true;
	}
	if (IsBool())
	{
		return !AsBool();
	}
	if (IsInt())
	{
		return AsInt() == 0;
	}
	return false;
}

bool Value::IsFunction() const
{
	return std::holds_alternative<HeapObject>(m_data) && std::get<HeapObject>(m_data)->GetType() == ObjType::FUNCTION;
}

bool Value::IsNative() const
{
	return std::holds_alternative<HeapObject>(m_data) && std::get<HeapObject>(m_data)->GetType() == ObjType::NATIVE;
}

bool Value::IsRef() const
{
	return std::holds_alternative<HeapObject>(m_data) && std::get<HeapObject>(m_data)->GetType() == ObjType::REF;
}

bool Value::IsArray() const
{
	return std::holds_alternative<HeapObject>(m_data) && std::get<HeapObject>(m_data)->GetType() == ObjType::ARRAY;
}

bool Value::IsSlice() const
{
	return std::holds_alternative<HeapObject>(m_data) && std::get<HeapObject>(m_data)->GetType() == ObjType::SLICE;
}

bool Value::IsSequence() const
{
	return IsArray() || IsSlice();
}

bool Value::IsStruct() const
{
	return std::holds_alternative<HeapObject>(m_data) && std::get<HeapObject>(m_data)->GetType() == ObjType::STRUCT;
}

RuntimeInt Value::AsInt() const
{
	return std::get<RuntimeInt>(m_data);
}

RuntimeFloat Value::AsFloat() const
{
	return std::get<RuntimeFloat>(m_data);
}

double Value::AsNumber() const
{
	if (IsInt())
	{
		return static_cast<double>(AsInt());
	}
	return AsFloat();
}

RuntimeBool Value::AsBool() const
{
	return std::get<RuntimeBool>(m_data);
}

const std::string& Value::AsString() const
{
	const auto obj = std::get<HeapObject>(m_data);
	return std::static_pointer_cast<ObjString>(obj)->GetData();
}

std::shared_ptr<ObjFunction> Value::AsFunction() const
{
	const auto obj = std::get<HeapObject>(m_data);
	return std::static_pointer_cast<ObjFunction>(obj);
}

std::shared_ptr<ObjNative> Value::AsNative() const
{
	const auto obj = std::get<HeapObject>(m_data);
	return std::static_pointer_cast<ObjNative>(obj);
}

std::shared_ptr<ObjRef> Value::AsRef() const
{
	const auto obj = std::get<HeapObject>(m_data);
	return std::static_pointer_cast<ObjRef>(obj);
}

std::shared_ptr<ObjArray> Value::AsArray() const
{
	const auto obj = std::get<HeapObject>(m_data);
	return std::static_pointer_cast<ObjArray>(obj);
}

std::shared_ptr<ObjSlice> Value::AsSlice() const
{
	const auto obj = std::get<HeapObject>(m_data);
	return std::static_pointer_cast<ObjSlice>(obj);
}

std::shared_ptr<ObjStruct> Value::AsStruct() const
{
	const auto obj = std::get<HeapObject>(m_data);
	return std::static_pointer_cast<ObjStruct>(obj);
}

Value Value::Dereference() const
{
	if (IsRef())
	{
		return AsRef()->Get();
	}
	return *this;
}

// TODO to refactor
void Value::Print() const
{
	std::visit([]<typename T>(const T& arg) {
		if constexpr (std::is_same_v<T, std::nullptr_t>)
		{
			std::printf("nil");
		}
		else if constexpr (std::is_same_v<T, RuntimeInt>)
		{
			std::printf("%" PRId64, arg);
		}
		else if constexpr (std::is_same_v<T, RuntimeFloat>)
		{
			std::printf("%g", arg);
		}
		else if constexpr (std::is_same_v<T, RuntimeBool>)
		{
			std::printf(arg ? "true" : "false");
		}
		else if constexpr (std::is_same_v<T, HeapObject>)
		{
			if (arg->GetType() == ObjType::STRING)
			{
				std::printf("%s", std::static_pointer_cast<ObjString>(arg)->GetData().c_str());
			}
			else if (arg->GetType() == ObjType::FUNCTION)
			{
				auto func = std::static_pointer_cast<ObjFunction>(arg);
				std::printf("<fn %s>", func->name->GetData().empty() ? "anonymous" : func->name->GetData().c_str());
			}
			else if (arg->GetType() == ObjType::NATIVE)
			{
				std::printf("<native fn>");
			}
			else if (arg->GetType() == ObjType::REF)
			{
				std::printf("<ref>");
			}
			else if (arg->GetType() == ObjType::ARRAY)
			{
				PrintSequence(*std::static_pointer_cast<ObjArray>(arg));
			}
			else if (arg->GetType() == ObjType::SLICE)
			{
				PrintSequence(*std::static_pointer_cast<ObjSlice>(arg));
			}
			else if (arg->GetType() == ObjType::STRUCT)
			{
				const auto obj = std::static_pointer_cast<ObjStruct>(arg);
				std::printf("<%s>", obj->GetTypeName().c_str());
			}
		}
	},
		m_data);
}

Value Value::operator-() const
{
	if (!IsNumber())
	{
		return {};
	}
	if (IsInt())
	{
		return Value(-AsInt());
	}
	return Value(-AsNumber());
}

Value Value::LogicalNot() const
{
	return Value(IsFalsey());
}

Value Value::operator+(const Value& other) const
{
	if (IsInt() && other.IsInt())
	{
		return Value(AsInt() + other.AsInt());
	}

	if (IsNumber() && other.IsNumber())
	{
		return Value(AsNumber() + other.AsNumber());
	}

	if (IsString() && other.IsString())
	{
		std::string concatenated = AsString() + other.AsString();
		return Value(std::make_shared<ObjString>(std::move(concatenated)));
	}

	return {};
}

Value Value::operator-(const Value& other) const
{
	if (!IsNumber() || !other.IsNumber())
	{
		return {};
	}
	if (IsInt() && other.IsInt())
	{
		return Value(AsInt() - other.AsInt());
	}
	return Value(AsNumber() - other.AsNumber());
}

Value Value::operator*(const Value& other) const
{
	if (!IsNumber() || !other.IsNumber())
	{
		return {};
	}
	if (IsInt() && other.IsInt())
	{
		return Value(AsInt() * other.AsInt());
	}
	return Value(AsNumber() * other.AsNumber());
}

Value Value::operator/(const Value& other) const
{
	if (!IsNumber() || !other.IsNumber())
	{
		return {};
	}
	if (IsInt() && other.IsInt())
	{
		if (other.AsInt() == 0)
		{
			return {};
		}
		return Value(AsInt() / other.AsInt());
	}
	return Value(AsNumber() / other.AsNumber());
}

Value Value::operator%(const Value& other) const
{
	if (!IsInt() || !other.IsInt())
	{
		return {};
	}
	if (other.AsInt() == 0)
	{
		return {};
	}
	return Value(AsInt() % other.AsInt());
}

Value Value::operator==(const Value& other) const
{
	if (IsNumber() && other.IsNumber())
	{
		return Value(AsNumber() == other.AsNumber());
	}

	if (m_data.index() != other.m_data.index())
	{
		return Value(false);
	}

	if (IsString() && other.IsString())
	{
		return Value(AsString() == other.AsString());
	}

	return Value(m_data == other.m_data);
}

Value Value::operator!=(const Value& other) const
{
	return Value(!(*this == other).AsBool());
}

Value Value::operator>(const Value& other) const
{
	if (!IsNumber() || !other.IsNumber())
	{
		return {};
	}
	return Value(AsNumber() > other.AsNumber());
}

Value Value::operator<(const Value& other) const
{
	if (!IsNumber() || !other.IsNumber())
	{
		return {};
	}
	return Value(AsNumber() < other.AsNumber());
}

Value Value::operator>=(const Value& other) const
{
	if (!IsNumber() || !other.IsNumber())
	{
		return {};
	}
	return Value(AsNumber() >= other.AsNumber());
}

Value Value::operator<=(const Value& other) const
{
	if (!IsNumber() || !other.IsNumber())
	{
		return {};
	}
	return Value(AsNumber() <= other.AsNumber());
}
