#pragma once

#include <memory>
#include <string>

enum class Type
{
	INT,
	FLOAT,
	BOOL,
	STRING,
	VOID,
	ERROR
};

class TypeDescriptor
{
public:
	TypeDescriptor();
	TypeDescriptor(Type scalarType);
	TypeDescriptor& operator=(Type scalarType);

	[[nodiscard]] static TypeDescriptor Array(int length, TypeDescriptor elementType);

	[[nodiscard]] Type GetScalarType() const;
	[[nodiscard]] bool IsArray() const;
	[[nodiscard]] int GetArrayLength() const;
	[[nodiscard]] const TypeDescriptor& GetElementType() const;
	[[nodiscard]] std::string ToString() const;

	friend bool operator==(const TypeDescriptor& left, const TypeDescriptor& right);
	friend bool operator!=(const TypeDescriptor& left, const TypeDescriptor& right);
	friend bool operator==(const TypeDescriptor& left, Type right);
	friend bool operator!=(const TypeDescriptor& left, Type right);
	friend bool operator==(Type left, const TypeDescriptor& right);
	friend bool operator!=(Type left, const TypeDescriptor& right);

private:
	Type m_scalarType = Type::ERROR;
	int m_arrayLength = 0;
	std::shared_ptr<TypeDescriptor> m_elementType;
};
