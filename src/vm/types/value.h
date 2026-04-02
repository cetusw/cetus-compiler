#pragma once
#include "src/vm/objects/Obj.h"
#include <cstddef>
#include <string>
#include <variant>

class ObjFunction;

class Value
{
public:
	Value();
	explicit Value(double value);
	explicit Value(bool value);
	explicit Value(HeapObject obj);

	[[nodiscard]] bool IsNull() const;
	[[nodiscard]] bool IsNumber() const;
	[[nodiscard]] bool IsBool() const;
	[[nodiscard]] bool IsString() const;
	[[nodiscard]] bool IsFalsey() const;
	[[nodiscard]] bool IsFunction() const;

	[[nodiscard]] double AsNumber() const;
	[[nodiscard]] bool AsBool() const;
	[[nodiscard]] const std::string& AsString() const;
	[[nodiscard]] std::shared_ptr<ObjFunction> AsFunction() const;

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
	std::variant<std::nullptr_t, double, bool, HeapObject> m_data;
};
