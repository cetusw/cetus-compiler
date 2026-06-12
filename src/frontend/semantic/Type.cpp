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
	m_kind = Kind::SCALAR;
	m_scalarType = scalarType;
	m_arrayLength = 0;
	m_elementType.reset();
	m_name.clear();
	return *this;
}

TypeDescriptor TypeDescriptor::Array(const int length, TypeDescriptor elementType)
{
	TypeDescriptor result;
	result.m_kind = Kind::ARRAY;
	result.m_scalarType = Type::ERROR;
	result.m_arrayLength = length;
	result.m_elementType = std::make_shared<TypeDescriptor>(std::move(elementType));
	return result;
}

TypeDescriptor TypeDescriptor::Slice(TypeDescriptor elementType)
{
	TypeDescriptor result;
	result.m_kind = Kind::SLICE;
	result.m_scalarType = Type::ERROR;
	result.m_elementType = std::make_shared<TypeDescriptor>(std::move(elementType));
	return result;
}

TypeDescriptor TypeDescriptor::Named(std::string name)
{
	TypeDescriptor result;
	result.m_kind = Kind::NAMED;
	result.m_scalarType = Type::ERROR;
	result.m_name = std::move(name);
	return result;
}

Type TypeDescriptor::GetScalarType() const
{
	return m_scalarType;
}

bool TypeDescriptor::IsArray() const
{
	return m_kind == Kind::ARRAY;
}

bool TypeDescriptor::IsSlice() const
{
	return m_kind == Kind::SLICE;
}

bool TypeDescriptor::IsSequence() const
{
	return IsArray() || IsSlice();
}

bool TypeDescriptor::IsIndexable() const
{
	return IsSequence() || *this == Type::STRING;
}

// TODO может отставить только IsSequence()
bool TypeDescriptor::IsSliceable() const
{
	return IsSequence();
}

bool TypeDescriptor::IsNamed() const
{
	return m_kind == Kind::NAMED;
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
	if (!IsSequence())
	{
		throw std::logic_error("Scalar type does not have element type.");
	}
	return *m_elementType;
}

TypeDescriptor TypeDescriptor::GetIndexResultType() const
{
	if (IsSequence())
	{
		return GetElementType();
	}
	if (*this == Type::STRING)
	{
		return Type::STRING;
	}

	throw std::logic_error("Type is not indexable.");
}

TypeDescriptor TypeDescriptor::GetSliceResultType() const
{
	if (!IsSliceable())
	{
		throw std::logic_error("Type is not sliceable.");
	}

	return Slice(GetElementType());
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
	if (IsSlice())
	{
		return "[]" + m_elementType->ToString();
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
	if (left.m_kind != right.m_kind)
	{
		return false;
	}

	switch (left.m_kind)
	{
	case TypeDescriptor::Kind::SCALAR:
		return left.m_scalarType == right.m_scalarType;
	case TypeDescriptor::Kind::ARRAY:
		return left.m_arrayLength == right.m_arrayLength && left.GetElementType() == right.GetElementType();
	case TypeDescriptor::Kind::SLICE:
		return left.GetElementType() == right.GetElementType();
	case TypeDescriptor::Kind::NAMED:
		return left.m_name == right.m_name;
	}

	return false;
}

bool operator!=(const TypeDescriptor& left, const TypeDescriptor& right)
{
	return !(left == right);
}

bool operator==(const TypeDescriptor& left, const Type right)
{
	return left.m_kind == TypeDescriptor::Kind::SCALAR && left.m_scalarType == right;
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
