#pragma once

#include "../Type.h"

#include <string>
#include <vector>

struct ParameterSignature
{
	TypeDescriptor type = Type::ERROR;
	bool isPointer = false;

	[[nodiscard]] TypeDescriptor GetEffectiveType() const
	{
		return isPointer ? TypeDescriptor::Pointer(type) : type;
	}
};

struct FieldSignature
{
	std::string name;
	TypeDescriptor type = Type::ERROR;
};

struct MethodSignature
{
	std::string name;
	std::string qualifiedName;
	TypeDescriptor type = Type::ERROR;
	bool receiverIsPointer = false;
	std::vector<ParameterSignature> parameters;
};

enum class SemanticSymbolKind
{
	VARIABLE,
	FUNCTION,
	BUILTIN_FUNCTION,
	TYPE
};

struct SemanticSymbol
{
	std::string name;
	TypeDescriptor type = Type::ERROR;
	SemanticSymbolKind kind = SemanticSymbolKind::VARIABLE;
	bool isStackBacked = false;
	bool isBorrowedReference = false;
	std::vector<ParameterSignature> parameters;
	std::vector<FieldSignature> fields;
	std::vector<MethodSignature> methods;
};
