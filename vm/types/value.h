#pragma once
#include <cstddef>
#include <variant>

class Value
{
public:
	Value();
	explicit Value(double value);
	explicit Value(bool value);

	[[nodiscard]] bool IsNumber() const;
	[[nodiscard]] bool IsBool() const;
	[[nodiscard]] bool IsNull() const;

	[[nodiscard]] double AsNumber() const;
	[[nodiscard]] bool AsBool() const;

	void Print() const;

	Value operator-() const;
	Value operator+(const Value& other) const;
	Value operator-(const Value& other) const;
	Value operator*(const Value& other) const;
	Value operator/(const Value& other) const;
	Value operator%(const Value& other) const;
	Value operator==(const Value& other) const;
	Value operator!=(const Value& other) const;
	Value operator>(const Value& other) const;
	Value operator<(const Value& other) const;
	Value operator>=(const Value& other) const;
	Value operator<=(const Value& other) const;

private:
	std::variant<std::nullptr_t, bool, double> m_data;
};
