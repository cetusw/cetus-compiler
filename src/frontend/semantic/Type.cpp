#include "Type.h"

#include <stdexcept>
#include <utility>

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
	m_name.clear();
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

TypeDescriptor TypeDescriptor::Named(std::string name)
{
	TypeDescriptor result;
	result.m_name = std::move(name);
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

bool TypeDescriptor::IsNamed() const
{
	return !m_name.empty();
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

const std::string& TypeDescriptor::GetName() const
{
	if (!IsNamed())
	{
		throw std::logic_error("Type is not a named type.");
	}
	return m_name;
}

std::string TypeDescriptor::ToString() const
{
	if (IsArray())
	{
		return "[" + std::to_string(m_arrayLength) + "]" + m_elementType->ToString();
	}
	if (IsNamed())
	{
		return m_name;
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
	if (left.IsNamed() != right.IsNamed())
	{
		return false;
	}
	if (!left.IsArray())
	{
		if (left.IsNamed())
		{
			return left.m_name == right.m_name;
		}
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
	return !left.IsArray() && !left.IsNamed() && left.m_scalarType == right;
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
