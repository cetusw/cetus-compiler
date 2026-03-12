#include "value.h"
#include <cstdio>
#include <cmath>

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

bool Value::IsNumber() const
{
	return std::holds_alternative<double>(m_data);
}

bool Value::IsBool() const
{
	return std::holds_alternative<bool>(m_data);
}

bool Value::IsNull() const
{
	return std::holds_alternative<std::nullptr_t>(m_data);
}

double Value::AsNumber() const
{
	return std::get<double>(m_data);
}

bool Value::AsBool() const
{
	return std::get<bool>(m_data);
}

void Value::Print() const
{
	std::visit([]<typename T>(T arg) {
		if constexpr (std::is_same_v<T, double>)
		{
			std::printf("%g", arg);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			std::printf(arg ? "true" : "false");
		}
		else if constexpr (std::is_same_v<T, std::nullptr_t>)
		{
			std::printf("nullptr");
		}
	},
		m_data);
}

Value Value::operator-() const
{
	return Value(-AsNumber());
}

Value Value::operator+(const Value& other) const
{
	return Value(AsNumber() + std::get<double>(other.m_data));
}

Value Value::operator-(const Value& other) const
{
	return Value(AsNumber() - std::get<double>(other.m_data));
}

Value Value::operator*(const Value& other) const
{
	return Value(AsNumber() * std::get<double>(other.m_data));
}

Value Value::operator/(const Value& other) const
{
	return Value(AsNumber() / std::get<double>(other.m_data));
}

Value Value::operator%(const Value& other) const
{
	return Value(std::fmod(AsNumber(), std::get<double>(other.m_data)));
}

Value Value::operator==(const Value& other) const
{
	return Value(m_data == other.m_data);
}

Value Value::operator>(const Value& other) const
{
	return Value(std::get<double>(m_data) > std::get<double>(other.m_data));
}

Value Value::operator<(const Value& other) const
{
	return Value(std::get<double>(m_data) < std::get<double>(other.m_data));
}

Value Value::operator>=(const Value& other) const
{
	return Value(std::get<double>(m_data) >= std::get<double>(other.m_data));
}

Value Value::operator<=(const Value& other) const
{
	return Value(std::get<double>(m_data) <= std::get<double>(other.m_data));
}
