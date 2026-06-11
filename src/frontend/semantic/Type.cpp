#include "Type.h"

#include <stdexcept>

TypeDescriptor::TypeDescriptor() = default;

TypeDescriptor::TypeDescriptor(const Type scalarType)
	: m_scalarType(scalarType)
{
}

TypeDescriptor& TypeDescriptor::operator=(const Type scalarType)
{
	m_scalarType = scalarType;
	m_arrayLength = 0;
	m_elementType.reset();
	return *this;
}

TypeDescriptor TypeDescriptor::Array(const int length, TypeDescriptor elementType)
{
	TypeDescriptor result;
	result.m_scalarType = Type::ERROR;
	result.m_arrayLength = length;
	result.m_elementType = std::make_shared<TypeDescriptor>(std::move(elementType));
	return result;
}

Type TypeDescriptor::GetScalarType() const
{
	return m_scalarType;
}

bool TypeDescriptor::IsArray() const
{
	return m_elementType != nullptr;
}

int TypeDescriptor::GetArrayLength() const
{
	if (!IsArray())
	{
		throw std::logic_error("Scalar type does not have array length.");
	}
	return m_arrayLength;
}

const TypeDescriptor& TypeDescriptor::GetElementType() const
{
	if (!IsArray())
	{
		throw std::logic_error("Scalar type does not have element type.");
	}
	return *m_elementType;
}

std::string TypeDescriptor::ToString() const
{
	if (IsArray())
	{
		return "[" + std::to_string(m_arrayLength) + "]" + m_elementType->ToString();
	}

	switch (m_scalarType)
	{
	case Type::INT: return "int";
	case Type::FLOAT: return "float";
	case Type::BOOL: return "bool";
	case Type::STRING: return "string";
	case Type::VOID: return "void";
	case Type::ERROR: return "error";
	}

	return "unknown";
}

bool operator==(const TypeDescriptor& left, const TypeDescriptor& right)
{
	if (left.IsArray() != right.IsArray())
	{
		return false;
	}
	if (!left.IsArray())
	{
		return left.m_scalarType == right.m_scalarType;
	}
	return left.m_arrayLength == right.m_arrayLength && left.GetElementType() == right.GetElementType();
}

bool operator!=(const TypeDescriptor& left, const TypeDescriptor& right)
{
	return !(left == right);
}

bool operator==(const TypeDescriptor& left, const Type right)
{
	return !left.IsArray() && left.m_scalarType == right;
}

bool operator!=(const TypeDescriptor& left, const Type right)
{
	return !(left == right);
}

bool operator==(const Type left, const TypeDescriptor& right)
{
	return right == left;
}

bool operator!=(const Type left, const TypeDescriptor& right)
{
	return !(left == right);
}
