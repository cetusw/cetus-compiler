#pragma once

#include "src/frontend/ast/ASTNode.h"
#include "src/frontend/lexical/core/Token.h"
#include <variant>
#include <vector>

struct NodeValue
{
	ASTNodePtr value;
};

struct ProgramValue
{
	std::unique_ptr<ProgramASTNode> value;
};

struct TokenValue
{
	Token value;
};

struct IdentifierListValue
{
	std::vector<std::string> values;
};

struct ExpressionListValue
{
	std::vector<ASTNodePtr> values;
};

struct ParameterListValue
{
	std::vector<FunctionParameter> values;
};

struct TypeValue
{
	TypeDescriptor value;
};

struct TypeListValue
{
	std::vector<TypeDescriptor> values;
};

struct StructFieldListValue
{
	std::vector<StructField> values;
};

struct FieldInitializerListValue
{
	std::vector<StructFieldInitializer> values;
};

struct StructLiteralTailValue
{
	std::vector<StructFieldInitializer> fieldInitializers;
};

struct EmptyStructLiteralTailValue
{
};

struct CompositeLiteralInitializerValue
{
	std::vector<ASTNodePtr> expressions;
};

struct StatementListValue
{
	std::vector<ASTNodePtr> values;
};

using AstSemanticValue = std::variant<
	std::monostate,
	NodeValue,
	ProgramValue,
	TokenValue,
	IdentifierListValue,
	ExpressionListValue,
	ParameterListValue,
	TypeValue,
	TypeListValue,
	StructFieldListValue,
	FieldInitializerListValue,
	StructLiteralTailValue,
	EmptyStructLiteralTailValue,
	CompositeLiteralInitializerValue,
	StatementListValue>;
