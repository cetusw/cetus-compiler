#pragma once

#include <memory>
#include <string>
#include <vector>

enum class Type
{
	INT,
	FLOAT,
	BOOL,
	STRING,
	NIL,
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
	[[nodiscard]] static TypeDescriptor Slice(TypeDescriptor elementType);
	[[nodiscard]] static TypeDescriptor Pointer(TypeDescriptor pointeeType);
	[[nodiscard]] static TypeDescriptor Tuple(std::vector<TypeDescriptor> elementTypes);
	[[nodiscard]] static TypeDescriptor Named(std::string name);

	[[nodiscard]] Type GetScalarType() const;
	[[nodiscard]] bool IsArray() const;
	[[nodiscard]] bool IsSlice() const;
	[[nodiscard]] bool IsPointer() const;
	[[nodiscard]] bool IsTuple() const;
	[[nodiscard]] bool IsSequence() const;
	[[nodiscard]] bool IsIndexable() const;
	[[nodiscard]] bool IsSliceable() const;
	[[nodiscard]] bool IsNamed() const;
	[[nodiscard]] bool IsNullable() const;
	[[nodiscard]] int GetArrayLength() const;
	[[nodiscard]] const TypeDescriptor& GetElementType() const;
	[[nodiscard]] const TypeDescriptor& GetPointeeType() const;
	[[nodiscard]] const std::vector<TypeDescriptor>& GetTupleElements() const;
	[[nodiscard]] TypeDescriptor GetIndexResultType() const;
	[[nodiscard]] TypeDescriptor GetSliceResultType() const;
	[[nodiscard]] const std::string& GetName() const;
	[[nodiscard]] std::string ToString() const;

	friend bool operator==(const TypeDescriptor& left, const TypeDescriptor& right);
	friend bool operator!=(const TypeDescriptor& left, const TypeDescriptor& right);
	friend bool operator==(const TypeDescriptor& left, Type right);
	friend bool operator!=(const TypeDescriptor& left, Type right);
	friend bool operator==(Type left, const TypeDescriptor& right);
	friend bool operator!=(Type left, const TypeDescriptor& right);

private:
	enum class Kind
	{
		SCALAR,
		ARRAY,
		SLICE,
		POINTER,
		TUPLE,
		NAMED
	};

	Kind m_kind = Kind::SCALAR;
	Type m_scalarType = Type::ERROR;
	int m_arrayLength = 0;
	std::shared_ptr<TypeDescriptor> m_elementType;
	std::shared_ptr<std::vector<TypeDescriptor>> m_tupleElements;
	std::string m_name;
};
