#include "value.h"

#include <cstdio>

Value::Value()
	: m_type(ValueType::Number)
	, m_asNumber(0.0)
{
}

Value::Value(const double value)
	: m_type(ValueType::Number)
	, m_asNumber(value)
{
}
bool Value::IsNumber() const
{
	return m_type == ValueType::Number;
}

double Value::AsNumber() const
{
	return m_asNumber;
}

void Value::Print() const
{
	switch (m_type)
	{
	case ValueType::Number:
		std::printf("%f", AsNumber());
		break;
	}
}

Value Value::operator-() const
{
	return Value(-AsNumber());
}

bool Value::operator==(const Value& other) const
{
	if (m_type != other.m_type)
	{
		return false;
	}
	return m_asNumber == other.m_asNumber;
}