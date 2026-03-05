#pragma once

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

	Value operator-() const;
	Value operator+(const Value& other) const;
	Value operator-(const Value& other) const;
	Value operator*(const Value& other) const;
	Value operator/(const Value& other) const;
	bool operator==(const Value& other) const;

private:
	ValueType m_type;
	double m_asNumber;
};
