#pragma once
#include <iosfwd>

enum class ValueType
{
	Number,
	// std::variant
};

class Value
{
public:
	Value();
	explicit Value(double value);

	bool IsNumber() const;
	double AsNumber() const;
	void Print() const;

	bool operator==(const Value& other) const;

private:
	ValueType m_type;
	double m_asNumber;
};
