#include "value.h"
#include "objects/ObjString.h"
#include <cassert>
#include <cmath>
#include <cstdio>

Value::Value()
	: m_data(nullptr)
{
}

Value::Value(const double value)
	: m_data(value)
{
}

Value::Value(bool value)
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
	return std::holds_alternative<double>(m_data);
}

bool Value::IsBool() const
{
	return std::holds_alternative<bool>(m_data);
}

bool Value::IsString() const
{
	return std::holds_alternative<HeapObject>(m_data) && std::get<HeapObject>(m_data)->GetType() == ObjType::STRING;
}

double Value::AsNumber() const
{
	return std::get<double>(m_data);
}

bool Value::AsBool() const
{
	return std::get<bool>(m_data);
}

const std::string& Value::AsString() const
{
	const auto obj = std::get<HeapObject>(m_data);
	return std::static_pointer_cast<ObjString>(obj)->GetData();
}

void Value::Print() const
{
	std::visit([]<typename T>(const T& arg) {
		if constexpr (std::is_same_v<T, std::nullptr_t>)
		{
			std::printf("nullptr");
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			std::printf("%g", arg);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			std::printf(arg ? "true" : "false");
		}
		else if constexpr (std::is_same_v<T, HeapObject>)
		{
			if (arg->GetType() == ObjType::STRING)
			{
				std::printf("%s", std::static_pointer_cast<ObjString>(arg)->GetData().c_str());
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
	return Value(-AsNumber());
}

Value Value::operator+(const Value& other) const
{
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
	return Value(AsNumber() - other.AsNumber());
}

Value Value::operator*(const Value& other) const
{
	if (!IsNumber() || !other.IsNumber())
	{
		return {};
	}
	return Value(AsNumber() * other.AsNumber());
}

Value Value::operator/(const Value& other) const
{
	if (!IsNumber() || !other.IsNumber())
	{
		return {};
	}
	return Value(AsNumber() / other.AsNumber());
}

Value Value::operator%(const Value& other) const
{
	if (!IsNumber() || !other.IsNumber())
	{
		return {};
	}
	return Value(std::fmod(AsNumber(), other.AsNumber()));
}

Value Value::operator==(const Value& other) const
{
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
