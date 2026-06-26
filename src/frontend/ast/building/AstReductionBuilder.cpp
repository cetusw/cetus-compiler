#include "AstReductionBuilder.h"

#include "src/frontend/ast/format/ExpressionFormatter.h"
#include <stdexcept>

AstSemanticValue AstReductionBuilder::Build(const ParserRule& rule, std::vector<AstSemanticValue> values)
{
	switch (rule.semanticTag)
	{
	case SemanticTag::PASS_EXPR:
		return PassNode(std::move(values), 0);
	case SemanticTag::PASS_TOKEN:
		return PassToken(values);
	case SemanticTag::GROUP:
		return PassNode(std::move(values), 1);
	case SemanticTag::UNARY:
		return BuildUnary(std::move(values));
	case SemanticTag::BINARY:
		return BuildBinary(std::move(values));
	case SemanticTag::BOOL_LITERAL:
		return BuildBoolLiteral(values);
	case SemanticTag::NIL_LITERAL:
		return BuildNilLiteral(values);
	case SemanticTag::INT_LITERAL:
		return BuildIntLiteral(values);
	case SemanticTag::FLOAT_LITERAL:
		return BuildFloatLiteral(values);
	case SemanticTag::STRING_LITERAL:
		return BuildStringLiteral(values);
	case SemanticTag::ARRAY_LITERAL:
		return BuildArrayLiteral(std::move(values));
	case SemanticTag::ARRAY_LITERAL_EMPTY:
		return BuildEmptyArrayLiteral(values);
	case SemanticTag::STRUCT_LITERAL:
		return BuildStructLiteral(std::move(values));
	case SemanticTag::STRUCT_LITERAL_EMPTY:
		return BuildEmptyStructLiteral(values);
	case SemanticTag::NAMED_POSTFIX:
		return BuildNamedPostfix(std::move(values));
	case SemanticTag::IDENTIFIER_TAIL:
		return BuildIdentifierTail(values);
	case SemanticTag::STRUCT_LITERAL_TAIL:
		return BuildStructLiteralTail(std::move(values));
	case SemanticTag::STRUCT_LITERAL_EMPTY_TAIL:
		return BuildEmptyStructLiteralTail(values);
	case SemanticTag::IDENTIFIER:
		return BuildIdentifier(values);
	case SemanticTag::ADDRESS_OF:
		return BuildAddressOf(std::move(values));
	case SemanticTag::ADDRESS_OF_STRUCT_LITERAL:
		return BuildAddressOfStructLiteral(std::move(values));
	case SemanticTag::ADDRESS_OF_STRUCT_LITERAL_EMPTY:
		return BuildAddressOfEmptyStructLiteral(values);
	case SemanticTag::IDENTIFIER_LIST:
		return BuildIdentifierList(std::move(values));
	case SemanticTag::IDENTIFIER_LIST_SINGLE:
		return BuildSingleIdentifierList(values);
	case SemanticTag::EXPRESSION_LIST:
		return BuildExpressionList(std::move(values));
	case SemanticTag::EXPRESSION_LIST_SINGLE:
		return BuildSingleExpressionList(std::move(values));
	case SemanticTag::TYPE_NAME:
		return BuildTypeName(values);
	case SemanticTag::ARRAY_TYPE:
		return BuildArrayType(values);
	case SemanticTag::SLICE_TYPE:
		return BuildSliceType(values);
	case SemanticTag::POINTER_TYPE:
		return BuildPointerType(values);
	case SemanticTag::SINGLE_RETURN_TYPE:
		return BuildSingleReturnType(values);
	case SemanticTag::TUPLE_RETURN_TYPE:
		return BuildTupleReturnType(std::move(values));
	case SemanticTag::TYPE_LIST:
		return BuildTypeList(std::move(values));
	case SemanticTag::TYPE_LIST_SINGLE:
		return BuildSingleTypeList(values);
	case SemanticTag::PARAM:
		return BuildParameter(values);
	case SemanticTag::POINTER_PARAM:
		return BuildPointerParameter(values);
	case SemanticTag::PARAM_LIST:
		return BuildParameterList(std::move(values));
	case SemanticTag::PARAM_LIST_SINGLE:
		return BuildSingleParameterList(std::move(values));
	case SemanticTag::CALL_NO_ARGS:
		return BuildCallNoArgs(values);
	case SemanticTag::CALL:
		return BuildCall(std::move(values));
	case SemanticTag::METHOD_CALL_NO_ARGS:
		return BuildMethodCallNoArgs(std::move(values));
	case SemanticTag::METHOD_CALL:
		return BuildMethodCall(std::move(values));
	case SemanticTag::MEMBER_ACCESS:
		return BuildMemberAccess(std::move(values));
	case SemanticTag::INDEX_ACCESS:
		return BuildIndexAccess(std::move(values));
	case SemanticTag::SLICE_EXPRESSION:
		return BuildSliceExpression(std::move(values));
	case SemanticTag::ASSIGNABLE_LIST:
		return BuildAssignableList(std::move(values));
	case SemanticTag::ASSIGNABLE_LIST_SINGLE:
		return BuildSingleAssignableList(std::move(values));
	case SemanticTag::ASSIGNMENT:
		return BuildAssignment(std::move(values));
	case SemanticTag::INCREMENT:
		return BuildIncrement(std::move(values));
	case SemanticTag::DECREMENT:
		return BuildDecrement(std::move(values));
	case SemanticTag::STRUCT_FIELD:
		return BuildStructField(values);
	case SemanticTag::STRUCT_FIELD_LIST:
		return BuildStructFieldList(std::move(values));
	case SemanticTag::STRUCT_FIELD_LIST_SINGLE:
		return BuildSingleStructFieldList(std::move(values));
	case SemanticTag::FIELD_INITIALIZER:
		return BuildFieldInitializer(std::move(values));
	case SemanticTag::FIELD_INITIALIZER_LIST:
		return BuildFieldInitializerList(std::move(values));
	case SemanticTag::FIELD_INITIALIZER_LIST_SINGLE:
		return BuildSingleFieldInitializerList(std::move(values));
	case SemanticTag::STRUCT_DECLARATION:
		return BuildStructDeclaration(std::move(values));
	case SemanticTag::SHORT_VAR_DECLARATION:
		return BuildShortVariableDeclaration(std::move(values));
	case SemanticTag::VAR_INFERRED_DECLARATION:
		return BuildVarInferredDeclaration(std::move(values));
	case SemanticTag::VAR_TYPED_DECLARATION:
		return BuildVarTypedDeclaration(std::move(values));
	case SemanticTag::VAR_TYPED_INITIALIZED_DECLARATION:
		return BuildVarTypedInitializedDeclaration(std::move(values));
	case SemanticTag::VAR_TYPED_DECLARATION_FULL:
		return BuildVarTypedDeclarationFull(std::move(values));
	case SemanticTag::VAR_TYPED_COMPOSITE_DECLARATION:
		return BuildVarTypedCompositeDeclaration(std::move(values));
	case SemanticTag::VAR_TYPED_EMPTY_COMPOSITE_DECLARATION:
		return BuildVarTypedEmptyCompositeDeclaration(std::move(values));
	case SemanticTag::EXPRESSION_STATEMENT:
		return BuildExpressionStatement(std::move(values));
	case SemanticTag::ASSERT_STATEMENT:
		return BuildAssertStatement(std::move(values));
	case SemanticTag::FORALL_STATEMENT:
		return BuildForAllStatement(std::move(values));
	case SemanticTag::EMPTY_STATEMENT:
		return BuildEmptyStatement(values);
	case SemanticTag::PROGRAM:
		return BuildProgram(std::move(values));
	case SemanticTag::PROGRAM_EMPTY:
		return BuildEmptyProgram(values);
	case SemanticTag::STATEMENT_LIST:
		return BuildStatementList(std::move(values));
	case SemanticTag::STATEMENT_LIST_SINGLE:
		return BuildSingleStatementList(std::move(values));
	case SemanticTag::IF:
		return BuildIf(std::move(values));
	case SemanticTag::IF_ELSE:
		return BuildIfElse(std::move(values));
	case SemanticTag::FOR_CONDITION:
		return BuildForCondition(std::move(values));
	case SemanticTag::FOR_CLASSIC:
		return BuildForClassic(std::move(values));
	case SemanticTag::BREAK:
		return BuildBreak(values);
	case SemanticTag::CONTINUE:
		return BuildContinue(values);
	case SemanticTag::RETURN_VOID:
		return BuildReturnVoid(values);
	case SemanticTag::RETURN_VALUE:
		return BuildReturnValue(std::move(values));
	case SemanticTag::TEST_DECLARATION:
		return BuildTestDeclaration(std::move(values));
	case SemanticTag::FUNCTION_VOID_NO_PARAMS:
		return BuildVoidFunctionNoParams(std::move(values));
	case SemanticTag::FUNCTION_RETURN_NO_PARAMS:
		return BuildReturnFunctionNoParams(std::move(values));
	case SemanticTag::FUNCTION_VOID:
		return BuildVoidFunction(std::move(values));
	case SemanticTag::FUNCTION_RETURN:
		return BuildReturnFunction(std::move(values));
	case SemanticTag::METHOD_VOID_NO_PARAMS:
		return BuildVoidMethodNoParams(std::move(values));
	case SemanticTag::METHOD_RETURN_NO_PARAMS:
		return BuildReturnMethodNoParams(std::move(values));
	case SemanticTag::METHOD_VOID:
		return BuildVoidMethod(std::move(values));
	case SemanticTag::METHOD_RETURN:
		return BuildReturnMethod(std::move(values));
	case SemanticTag::BLOCK:
		return BuildBlock(std::move(values));
	case SemanticTag::BLOCK_EMPTY:
		return BuildEmptyBlock(values);
	case SemanticTag::NONE:
		throw std::logic_error("Missing semantic tag for reduced parser rule.");
	}
	throw std::runtime_error("Unhandled semantic tag.");
}

void AstReductionBuilder::RequireValueCount(
	const std::vector<AstSemanticValue>& values,
	const std::size_t expectedCount,
	const char* actionName)
{
	if (values.size() != expectedCount)
	{
		throw std::logic_error(std::string(actionName) + " expects " + std::to_string(expectedCount) + " semantic values.");
	}
}

AstSemanticValue AstReductionBuilder::BuildBinary(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Binary reduction");
	return NodeValue{
		std::make_unique<BinaryASTNode>(
			TakeNode(values, 0),
			ToBinaryOperator(TakeToken(values, 1).type),
			TakeNode(values, 2))
	};
}

AstSemanticValue AstReductionBuilder::BuildUnary(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 2, "Unary reduction");
	return NodeValue{
		std::make_unique<UnaryASTNode>(
			ToUnaryOperator(TakeToken(values, 0).type),
			TakeNode(values, 1))
	};
}

AstSemanticValue AstReductionBuilder::BuildBoolLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Bool literal reduction");
	return NodeValue{
		std::make_unique<BoolLiteralASTNode>(
			TakeToken(values, 0).type == TokenType::TRUE)
	};
}

AstSemanticValue AstReductionBuilder::BuildNilLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Nil literal reduction");
	return NodeValue{
		std::make_unique<NilLiteralASTNode>()
	};
}

AstSemanticValue AstReductionBuilder::BuildIntLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Int literal reduction");
	return NodeValue{
		std::make_unique<IntLiteralASTNode>(
			TakeToken(values, 0).lexeme)
	};
}

AstSemanticValue AstReductionBuilder::BuildFloatLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Float literal reduction");
	return NodeValue{
		std::make_unique<FloatLiteralASTNode>(
			TakeToken(values, 0).lexeme)
	};
}

AstSemanticValue AstReductionBuilder::BuildStringLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "String literal reduction");
	return NodeValue{
		std::make_unique<StringLiteralASTNode>(
			TakeToken(values, 0).lexeme)
	};
}

AstSemanticValue AstReductionBuilder::BuildArrayLiteral(std::vector<AstSemanticValue> values)
{
	if (values.size() == 6)
	{
		return NodeValue{
			std::make_unique<ArrayLiteralASTNode>(
				TypeDescriptor::Slice(TakeType(values, 2)),
				TakeExpressionList(values, 4))
		};
	}

	if (values.size() == 7
		&& TakeToken(values, 1).type == TokenType::RBRACKET)
	{
		return NodeValue{
			std::make_unique<ArrayLiteralASTNode>(
				TypeDescriptor::Slice(TakeType(values, 2)),
				TakeExpressionList(values, 4))
		};
	}

	if (values.size() == 7)
	{
		const int length = std::stoi(TakeToken(values, 1).lexeme);
		if (length <= 0)
		{
			throw std::runtime_error("Array length must be source.");
		}

		return NodeValue{
			std::make_unique<ArrayLiteralASTNode>(
				TypeDescriptor::Array(length, TakeType(values, 3)),
				TakeExpressionList(values, 5))
		};
	}

	if (values.size() == 8)
	{
		const int length = std::stoi(TakeToken(values, 1).lexeme);
		if (length <= 0)
		{
			throw std::runtime_error("Array length must be source.");
		}

		return NodeValue{
			std::make_unique<ArrayLiteralASTNode>(
				TypeDescriptor::Array(length, TakeType(values, 3)),
				TakeExpressionList(values, 5))
		};
	}

	throw std::logic_error("Array literal reduction expects 6, 7 or 8 semantic values.");
}

AstSemanticValue AstReductionBuilder::BuildEmptyArrayLiteral(const std::vector<AstSemanticValue>& values)
{
	if (values.size() == 5)
	{
		return NodeValue{
			std::make_unique<ArrayLiteralASTNode>(
				TypeDescriptor::Slice(TakeType(values, 2)),
				std::vector<ASTNodePtr>{})
		};
	}

	if (values.size() == 6)
	{
		const int length = std::stoi(TakeToken(values, 1).lexeme);
		if (length <= 0)
		{
			throw std::runtime_error("Array length must be source.");
		}

		return NodeValue{
			std::make_unique<ArrayLiteralASTNode>(
				TypeDescriptor::Array(length, TakeType(values, 3)),
				std::vector<ASTNodePtr>{})
		};
	}

	throw std::logic_error("Empty array literal reduction expects 5 or 6 semantic values.");
}

AstSemanticValue AstReductionBuilder::BuildStructLiteral(std::vector<AstSemanticValue> values)
{
	if (values.size() == 4 || values.size() == 5)
	{
		return NodeValue{
			std::make_unique<StructLiteralASTNode>(
				TakeToken(values, 0).lexeme,
				TakeFieldInitializerList(values, 2))
		};
	}

	throw std::logic_error("Struct literal reduction expects 4 or 5 semantic values.");
}

AstSemanticValue AstReductionBuilder::BuildEmptyStructLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 3, "Empty struct literal reduction");

	return NodeValue{
		std::make_unique<StructLiteralASTNode>(
			TakeToken(values, 0).lexeme,
			std::vector<StructFieldInitializer>{})
	};
}

AstSemanticValue AstReductionBuilder::BuildIdentifier(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Identifier reduction");
	return NodeValue{
		std::make_unique<IdentifierASTNode>(
			TakeToken(values, 0).lexeme)
	};
}

AstSemanticValue AstReductionBuilder::BuildNamedPostfix(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 2, "Named postfix reduction");

	const std::string typeOrName = TakeToken(values, 0).lexeme;

	if (std::holds_alternative<StructLiteralTailValue>(values[1]))
	{
		return NodeValue{
			std::make_unique<StructLiteralASTNode>(
				typeOrName,
				TakeFieldInitializerList(values, 1))
		};
	}

	if (std::holds_alternative<EmptyStructLiteralTailValue>(values[1]))
	{
		return NodeValue{
			std::make_unique<StructLiteralASTNode>(
				typeOrName,
				std::vector<StructFieldInitializer>{})
		};
	}

	return NodeValue{
		std::make_unique<IdentifierASTNode>(typeOrName)
	};
}

AstSemanticValue AstReductionBuilder::BuildIdentifierTail(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 0, "Identifier tail reduction");
	return std::monostate{};
}

AstSemanticValue AstReductionBuilder::BuildStructLiteralTail(std::vector<AstSemanticValue> values)
{
	if (values.size() != 3 && values.size() != 4)
	{
		throw std::logic_error("Struct literal tail reduction expects 3 or 4 semantic values.");
	}

	return StructLiteralTailValue{
		TakeFieldInitializerList(values, 1)
	};
}

AstSemanticValue AstReductionBuilder::BuildEmptyStructLiteralTail(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 2, "Empty struct literal tail reduction");
	return EmptyStructLiteralTailValue{};
}

AstSemanticValue AstReductionBuilder::BuildAddressOf(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 2, "Address-of reduction");

	return NodeValue{
		std::make_unique<AddressOfASTNode>(
			TakeNode(values, 1))
	};
}

AstSemanticValue AstReductionBuilder::BuildAddressOfStructLiteral(std::vector<AstSemanticValue> values)
{
	if (values.size() == 5 || values.size() == 6)
	{
		return NodeValue{
			std::make_unique<AddressOfASTNode>(
				std::make_unique<StructLiteralASTNode>(
					TakeToken(values, 1).lexeme,
					TakeFieldInitializerList(values, 3)))
		};
	}

	throw std::logic_error("Address-of struct literal reduction expects 5 or 6 semantic values.");
}

AstSemanticValue AstReductionBuilder::BuildAddressOfEmptyStructLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 4, "Address-of empty struct literal reduction");

	return NodeValue{
		std::make_unique<AddressOfASTNode>(
			std::make_unique<StructLiteralASTNode>(
				TakeToken(values, 1).lexeme,
				std::vector<StructFieldInitializer>{}))
	};
}

AstSemanticValue AstReductionBuilder::BuildIdentifierList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Identifier list reduction");

	std::vector<std::string> identifiers = TakeIdentifierList(values, 0);
	identifiers.push_back(TakeToken(values, 2).lexeme);

	return IdentifierListValue{ std::move(identifiers) };
}

AstSemanticValue AstReductionBuilder::BuildSingleIdentifierList(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Single identifier list reduction");

	return IdentifierListValue{
		{ TakeToken(values, 0).lexeme }
	};
}

AstSemanticValue AstReductionBuilder::BuildExpressionList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Expression list reduction");

	std::vector<ASTNodePtr> expressions = TakeExpressionList(values, 0);
	expressions.push_back(TakeNode(values, 2));

	return ExpressionListValue{ std::move(expressions) };
}

AstSemanticValue AstReductionBuilder::BuildSingleExpressionList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Single expression list reduction");

	std::vector<ASTNodePtr> expressions;
	expressions.push_back(TakeNode(values, 0));

	return ExpressionListValue{ std::move(expressions) };
}

AstSemanticValue AstReductionBuilder::BuildTypeName(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Type name reduction");

	const std::string& typeName = TakeToken(values, 0).lexeme;

	if (typeName == "int")
	{
		return TypeValue{ Type::INT };
	}
	if (typeName == "float")
	{
		return TypeValue{ Type::FLOAT };
	}
	if (typeName == "bool")
	{
		return TypeValue{ Type::BOOL };
	}
	if (typeName == "string")
	{
		return TypeValue{ Type::STRING };
	}

	return TypeValue{ TypeDescriptor::Named(typeName) };
}

AstSemanticValue AstReductionBuilder::BuildArrayType(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 4, "Array type reduction");

	const int length = std::stoi(TakeToken(values, 1).lexeme);
	if (length <= 0)
	{
		throw std::runtime_error("Array length must be source.");
	}

	return TypeValue{
		TypeDescriptor::Array(length, TakeType(values, 3))
	};
}

AstSemanticValue AstReductionBuilder::BuildSliceType(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 3, "Slice type reduction");

	return TypeValue{
		TypeDescriptor::Slice(TakeType(values, 2))
	};
}

AstSemanticValue AstReductionBuilder::BuildPointerType(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 2, "Pointer type reduction");

	return TypeValue{
		TypeDescriptor::Pointer(TakeType(values, 1))
	};
}

AstSemanticValue AstReductionBuilder::BuildSingleReturnType(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Single return type reduction");

	return TypeValue{ TakeType(values, 0) };
}

AstSemanticValue AstReductionBuilder::BuildTupleReturnType(std::vector<AstSemanticValue> values)
{
	if (values.size() != 3 && values.size() != 4)
	{
		throw std::logic_error("Tuple return type reduction expects 3 or 4 semantic values.");
	}

	return TypeValue{
		TypeDescriptor::Tuple(TakeTypeList(values, 1))
	};
}

AstSemanticValue AstReductionBuilder::BuildTypeList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Type list reduction");

	std::vector<TypeDescriptor> types = TakeTypeList(values, 0);
	std::vector<TypeDescriptor> nextType = TakeTypeList(values, 2);

	types.push_back(std::move(nextType.front()));

	return TypeListValue{ std::move(types) };
}

AstSemanticValue AstReductionBuilder::BuildSingleTypeList(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Single type list reduction");

	return TypeListValue{
		{ TakeType(values, 0) }
	};
}

AstSemanticValue AstReductionBuilder::BuildParameter(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 2, "Parameter reduction");

	const TypeDescriptor parameterType = TakeType(values, 1);

	if (parameterType.IsPointer())
	{
		return ParameterListValue{
			{ FunctionParameter{
				TakeToken(values, 0).lexeme,
				parameterType.GetPointeeType(),
				true } }
		};
	}

	return ParameterListValue{
		{ FunctionParameter{
			TakeToken(values, 0).lexeme,
			parameterType } }
	};
}

AstSemanticValue AstReductionBuilder::BuildPointerParameter(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 3, "Pointer parameter reduction");

	return ParameterListValue{
		{ FunctionParameter{
			TakeToken(values, 0).lexeme,
			TakeType(values, 2),
			true } }
	};
}

AstSemanticValue AstReductionBuilder::BuildParameterList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Parameter list reduction");

	std::vector<FunctionParameter> parameters = TakeParameterList(values, 0);

	std::vector<FunctionParameter> nextParameter = TakeParameterList(values, 2);

	parameters.push_back(std::move(nextParameter.front()));

	return ParameterListValue{ std::move(parameters) };
}

AstSemanticValue AstReductionBuilder::BuildSingleParameterList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Single parameter list reduction");

	return ParameterListValue{
		TakeParameterList(values, 0)
	};
}

AstSemanticValue AstReductionBuilder::BuildCallNoArgs(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 3, "Function call without arguments reduction");

	return NodeValue{
		std::make_unique<CallExpressionASTNode>(
			TakeToken(values, 0).lexeme,
			std::vector<ASTNodePtr>{})
	};
}

AstSemanticValue AstReductionBuilder::BuildCall(std::vector<AstSemanticValue> values)
{
	if (values.size() != 4 && values.size() != 5)
	{
		throw std::logic_error("Function call reduction expects 4 or 5 semantic values.");
	}

	return NodeValue{
		std::make_unique<CallExpressionASTNode>(
			TakeToken(values, 0).lexeme,
			TakeExpressionList(values, 2))
	};
}

AstSemanticValue AstReductionBuilder::BuildMethodCallNoArgs(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 5, "Method call without arguments reduction");

	return NodeValue{
		std::make_unique<CallExpressionASTNode>(
			TakeNode(values, 0),
			TakeToken(values, 2).lexeme,
			std::vector<ASTNodePtr>{})
	};
}

AstSemanticValue AstReductionBuilder::BuildMethodCall(std::vector<AstSemanticValue> values)
{
	if (values.size() != 6 && values.size() != 7)
	{
		throw std::logic_error("Method call reduction expects 6 or 7 semantic values.");
	}

	return NodeValue{
		std::make_unique<CallExpressionASTNode>(
			TakeNode(values, 0),
			TakeToken(values, 2).lexeme,
			TakeExpressionList(values, 4))
	};
}

AstSemanticValue AstReductionBuilder::BuildMemberAccess(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Member access reduction");

	return NodeValue{
		std::make_unique<MemberAccessASTNode>(
			TakeNode(values, 0),
			TakeToken(values, 2).lexeme)
	};
}

AstSemanticValue AstReductionBuilder::BuildIndexAccess(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 4, "Index access reduction");

	return NodeValue{
		std::make_unique<IndexASTNode>(
			TakeNode(values, 0),
			TakeNode(values, 2))
	};
}

AstSemanticValue AstReductionBuilder::BuildSliceExpression(std::vector<AstSemanticValue> values)
{
	if (values.size() == 4)
	{
		return NodeValue{
			std::make_unique<SliceExpressionASTNode>(
				TakeNode(values, 0),
				nullptr,
				nullptr)
		};
	}

	if (values.size() == 5)
	{
		if (const auto* token = std::get_if<TokenValue>(&values[2]);
			token && token->value.type == TokenType::COLON)
		{
			return NodeValue{
				std::make_unique<SliceExpressionASTNode>(
					TakeNode(values, 0),
					nullptr,
					TakeNode(values, 3))
			};
		}

		return NodeValue{
			std::make_unique<SliceExpressionASTNode>(
				TakeNode(values, 0),
				TakeNode(values, 2),
				nullptr)
		};
	}

	if (values.size() == 6)
	{
		return NodeValue{
			std::make_unique<SliceExpressionASTNode>(
				TakeNode(values, 0),
				TakeNode(values, 2),
				TakeNode(values, 4))
		};
	}

	throw std::logic_error("Slice expression reduction expects 4, 5 or 6 semantic values.");
}

AstSemanticValue AstReductionBuilder::BuildAssignableList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Assignable list reduction");

	std::vector<ASTNodePtr> targets = TakeExpressionList(values, 0);
	targets.push_back(TakeNode(values, 2));

	return ExpressionListValue{ std::move(targets) };
}

AstSemanticValue AstReductionBuilder::BuildSingleAssignableList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Single assignable list reduction");

	std::vector<ASTNodePtr> targets;
	targets.push_back(TakeNode(values, 0));

	return ExpressionListValue{ std::move(targets) };
}

AstSemanticValue AstReductionBuilder::BuildAssignment(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Assignment reduction");

	return NodeValue{
		std::make_unique<AssignmentASTNode>(
			TakeExpressionList(values, 0),
			TakeExpressionList(values, 2))
	};
}

AstSemanticValue AstReductionBuilder::BuildIncrement(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 2, "Increment reduction");

	return NodeValue{
		std::make_unique<IncrementASTNode>(
			TakeNode(values, 0))
	};
}

AstSemanticValue AstReductionBuilder::BuildDecrement(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 2, "Decrement reduction");

	return NodeValue{
		std::make_unique<DecrementASTNode>(
			TakeNode(values, 0))
	};
}

AstSemanticValue AstReductionBuilder::BuildStructField(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 3, "Struct field reduction");

	return StructFieldListValue{
		{ StructField{
			TakeToken(values, 0).lexeme,
			TakeType(values, 1) } }
	};
}

AstSemanticValue AstReductionBuilder::BuildStructFieldList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 2, "Struct field list reduction");

	std::vector<StructField> fields = TakeStructFieldList(values, 0);
	std::vector<StructField> nextField = TakeStructFieldList(values, 1);

	fields.push_back(std::move(nextField.front()));

	return StructFieldListValue{ std::move(fields) };
}

AstSemanticValue AstReductionBuilder::BuildSingleStructFieldList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Single struct field list reduction");

	return StructFieldListValue{
		TakeStructFieldList(values, 0)
	};
}

AstSemanticValue AstReductionBuilder::BuildFieldInitializer(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Field initializer reduction");

	std::vector<StructFieldInitializer> initializers;
	initializers.push_back(StructFieldInitializer{
		TakeToken(values, 0).lexeme,
		TakeNode(values, 2) });

	return FieldInitializerListValue{ std::move(initializers) };
}

AstSemanticValue AstReductionBuilder::BuildFieldInitializerList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Field initializer list reduction");

	std::vector<StructFieldInitializer> initializers = TakeFieldInitializerList(values, 0);

	std::vector<StructFieldInitializer> nextInitializer = TakeFieldInitializerList(values, 2);

	initializers.push_back(std::move(nextInitializer.front()));

	return FieldInitializerListValue{ std::move(initializers) };
}

AstSemanticValue AstReductionBuilder::BuildSingleFieldInitializerList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Single field initializer list reduction");

	return FieldInitializerListValue{
		TakeFieldInitializerList(values, 0)
	};
}

AstSemanticValue AstReductionBuilder::BuildStructDeclaration(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 6, "Struct declaration reduction");

	return NodeValue{
		std::make_unique<StructDeclarationASTNode>(
			TakeToken(values, 1).lexeme,
			TakeStructFieldList(values, 4))
	};
}

AstSemanticValue AstReductionBuilder::BuildShortVariableDeclaration(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Short variable declaration reduction");

	return NodeValue{
		std::make_unique<ShortVariableDeclarationASTNode>(
			TakeIdentifierNamesFromTargets(values),
			TakeExpressionList(values, 2))
	};
}

AstSemanticValue AstReductionBuilder::BuildVarInferredDeclaration(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 4, "Inferred variable declaration reduction");

	return NodeValue{
		std::make_unique<VariableDeclarationASTNode>(
			TakeIdentifierList(values, 1),
			std::nullopt,
			TakeExpressionList(values, 3))
	};
}

AstSemanticValue AstReductionBuilder::BuildVarTypedDeclaration(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Typed variable declaration reduction");

	return NodeValue{
		std::make_unique<VariableDeclarationASTNode>(
			TakeIdentifierList(values, 1),
			TakeType(values, 2),
			std::vector<ASTNodePtr>{})
	};
}

AstSemanticValue AstReductionBuilder::BuildVarTypedInitializedDeclaration(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 5, "Typed initialized variable declaration reduction");

	return NodeValue{
		std::make_unique<VariableDeclarationASTNode>(
			TakeIdentifierList(values, 1),
			TakeType(values, 2),
			TakeExpressionList(values, 4))
	};
}

AstSemanticValue AstReductionBuilder::BuildVarTypedDeclarationFull(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 4, "Typed variable declaration full reduction");

	const TypeDescriptor declaredType = TakeType(values, 2);
	const std::vector<std::string> names = TakeIdentifierList(values, 1);

	std::vector<ASTNodePtr> initializers;

	if (auto* compositeInitializer = std::get_if<CompositeLiteralInitializerValue>(&values[3]))
	{
		initializers.push_back(std::make_unique<ArrayLiteralASTNode>(
			declaredType,
			std::move(compositeInitializer->expressions)));
	}
	else
	{
		initializers = TakeExpressionList(values, 3);
	}

	return NodeValue{
		std::make_unique<VariableDeclarationASTNode>(
			names,
			declaredType,
			std::move(initializers))
	};
}

AstSemanticValue AstReductionBuilder::BuildVarTypedCompositeDeclaration(std::vector<AstSemanticValue> values)
{
	if (values.size() != 6 && values.size() != 7)
	{
		throw std::logic_error("Typed composite variable declaration reduction expects 6 or 7 semantic values.");
	}

	std::vector<ASTNodePtr> initializers;
	initializers.push_back(std::make_unique<ArrayLiteralASTNode>(
		TakeType(values, 2),
		TakeExpressionList(values, 4)));

	return NodeValue{
		std::make_unique<VariableDeclarationASTNode>(
			TakeIdentifierList(values, 1),
			TakeType(values, 2),
			std::move(initializers))
	};
}

AstSemanticValue AstReductionBuilder::BuildVarTypedEmptyCompositeDeclaration(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 5, "Typed empty composite variable declaration reduction");

	std::vector<ASTNodePtr> initializers;
	initializers.push_back(std::make_unique<ArrayLiteralASTNode>(
		TakeType(values, 2),
		std::vector<ASTNodePtr>{}));

	return NodeValue{
		std::make_unique<VariableDeclarationASTNode>(
			TakeIdentifierList(values, 1),
			TakeType(values, 2),
			std::move(initializers))
	};
}

AstSemanticValue AstReductionBuilder::BuildExpressionStatement(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Expression statement reduction");

	return NodeValue{
		std::make_unique<ExpressionStatementASTNode>(
			TakeNode(values, 0))
	};
}

AstSemanticValue AstReductionBuilder::BuildAssertStatement(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 5, "Assert statement reduction");

	Token assertToken = TakeToken(values, 0);
	ASTNodePtr condition = TakeNode(values, 2);
	std::string sourceText = ExpressionFormatter::Format(*condition);

	return NodeValue{
		std::make_unique<AssertStatementASTNode>(
			std::move(condition),
			assertToken.line,
			std::move(sourceText))
	};
}

AstSemanticValue AstReductionBuilder::BuildForAllStatement(std::vector<AstSemanticValue> values)
{
	if (values.size() != 5 && values.size() != 6)
	{
		throw std::logic_error("Forall statement reduction expects 5 or 6 semantic values.");
	}

	return NodeValue{
		std::make_unique<ForAllStatementASTNode>(
			TakeParameterList(values, 2),
			TakeNode(values, values.size() - 1))
	};
}

AstSemanticValue AstReductionBuilder::BuildEmptyStatement(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Empty statement reduction");

	return NodeValue{
		std::make_unique<EmptyStatementASTNode>()
	};
}

AstSemanticValue AstReductionBuilder::BuildProgram(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Program reduction");

	return ProgramValue{
		std::make_unique<ProgramASTNode>(
			std::make_unique<StatementListASTNode>(
				TakeStatementList(values, 0)))
	};
}

AstSemanticValue AstReductionBuilder::BuildEmptyProgram(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 0, "Empty program reduction");

	return ProgramValue{
		std::make_unique<ProgramASTNode>(
			std::make_unique<StatementListASTNode>(
				std::vector<ASTNodePtr>{}))
	};
}

AstSemanticValue AstReductionBuilder::BuildStatementList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 2, "Statement list reduction");

	std::vector<ASTNodePtr> statements = TakeStatementList(values, 0);
	statements.push_back(TakeNode(values, 1));

	return StatementListValue{ std::move(statements) };
}

AstSemanticValue AstReductionBuilder::BuildSingleStatementList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Single statement list reduction");

	std::vector<ASTNodePtr> statements;
	statements.push_back(TakeNode(values, 0));

	return StatementListValue{ std::move(statements) };
}

AstSemanticValue AstReductionBuilder::BuildBlock(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Block reduction");

	return NodeValue{
		std::make_unique<BlockASTNode>(
			std::make_unique<StatementListASTNode>(
				TakeStatementList(values, 1)))
	};
}

AstSemanticValue AstReductionBuilder::BuildEmptyBlock(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 2, "Empty block reduction");

	return NodeValue{
		std::make_unique<BlockASTNode>(
			std::make_unique<StatementListASTNode>(
				std::vector<ASTNodePtr>{}))
	};
}

AstSemanticValue AstReductionBuilder::BuildIf(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "If reduction");

	return NodeValue{
		std::make_unique<IfASTNode>(
			TakeNode(values, 1),
			TakeNode(values, 2))
	};
}

AstSemanticValue AstReductionBuilder::BuildIfElse(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 5, "If/else reduction");

	return NodeValue{
		std::make_unique<IfASTNode>(
			TakeNode(values, 1),
			TakeNode(values, 2),
			TakeNode(values, 4))
	};
}

AstSemanticValue AstReductionBuilder::BuildForCondition(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "For condition reduction");

	return NodeValue{
		std::make_unique<ForASTNode>(
			nullptr,
			TakeNode(values, 1),
			nullptr,
			TakeNode(values, 2))
	};
}

AstSemanticValue AstReductionBuilder::BuildForClassic(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 7, "Classic for reduction");

	return NodeValue{
		std::make_unique<ForASTNode>(
			TakeNode(values, 1),
			TakeNode(values, 3),
			TakeNode(values, 5),
			TakeNode(values, 6))
	};
}

AstSemanticValue AstReductionBuilder::BuildBreak(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 2, "Break reduction");

	return NodeValue{
		std::make_unique<BreakASTNode>()
	};
}

AstSemanticValue AstReductionBuilder::BuildContinue(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 2, "Continue reduction");

	return NodeValue{
		std::make_unique<ContinueASTNode>()
	};
}

AstSemanticValue AstReductionBuilder::BuildReturnVoid(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 2, "Return void reduction");

	return NodeValue{
		std::make_unique<ReturnASTNode>()
	};
}

AstSemanticValue AstReductionBuilder::BuildReturnValue(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Return value reduction");

	return NodeValue{
		std::make_unique<ReturnASTNode>(
			TakeExpressionList(values, 1))
	};
}

AstSemanticValue AstReductionBuilder::BuildTestDeclaration(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Test declaration reduction");

	return NodeValue{
		std::make_unique<TestDeclarationASTNode>(
			TakeToken(values, 1).lexeme,
			TakeNode(values, 2))
	};
}

AstSemanticValue AstReductionBuilder::BuildVoidFunctionNoParams(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 5, "Void function declaration reduction");

	return NodeValue{
		std::make_unique<FunctionDeclarationASTNode>(
			TakeToken(values, 1).lexeme,
			std::vector<FunctionParameter>{},
			std::nullopt,
			TakeNode(values, 4))
	};
}

AstSemanticValue AstReductionBuilder::BuildReturnFunctionNoParams(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 6, "Returning function declaration reduction");

	return NodeValue{
		std::make_unique<FunctionDeclarationASTNode>(
			TakeToken(values, 1).lexeme,
			std::vector<FunctionParameter>{},
			TakeType(values, 4),
			TakeNode(values, 5))
	};
}

AstSemanticValue AstReductionBuilder::BuildVoidFunction(std::vector<AstSemanticValue> values)
{
	if (values.size() != 6 && values.size() != 7)
	{
		throw std::logic_error("Void function declaration reduction expects 6 or 7 semantic values.");
	}

	return NodeValue{
		std::make_unique<FunctionDeclarationASTNode>(
			TakeToken(values, 1).lexeme,
			TakeParameterList(values, 3),
			std::nullopt,
			TakeNode(values, values.size() - 1))
	};
}

AstSemanticValue AstReductionBuilder::BuildReturnFunction(std::vector<AstSemanticValue> values)
{
	if (values.size() != 7 && values.size() != 8)
	{
		throw std::logic_error("Returning function declaration reduction expects 7 or 8 semantic values.");
	}

	return NodeValue{
		std::make_unique<FunctionDeclarationASTNode>(
			TakeToken(values, 1).lexeme,
			TakeParameterList(values, 3),
			TakeType(values, values.size() - 2),
			TakeNode(values, values.size() - 1))
	};
}

AstSemanticValue AstReductionBuilder::BuildVoidMethodNoParams(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 8, "Void method declaration reduction");

	return NodeValue{
		std::make_unique<FunctionDeclarationASTNode>(
			TakeParameterList(values, 2).front(),
			TakeToken(values, 4).lexeme,
			std::vector<FunctionParameter>{},
			std::nullopt,
			TakeNode(values, 7))
	};
}

AstSemanticValue AstReductionBuilder::BuildReturnMethodNoParams(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 9, "Returning method declaration reduction");

	return NodeValue{
		std::make_unique<FunctionDeclarationASTNode>(
			TakeParameterList(values, 2).front(),
			TakeToken(values, 4).lexeme,
			std::vector<FunctionParameter>{},
			TakeType(values, 7),
			TakeNode(values, 8))
	};
}

AstSemanticValue AstReductionBuilder::BuildVoidMethod(std::vector<AstSemanticValue> values)
{
	if (values.size() != 9 && values.size() != 10)
	{
		throw std::logic_error("Void method declaration reduction expects 9 or 10 semantic values.");
	}

	return NodeValue{
		std::make_unique<FunctionDeclarationASTNode>(
			TakeParameterList(values, 2).front(),
			TakeToken(values, 4).lexeme,
			TakeParameterList(values, 6),
			std::nullopt,
			TakeNode(values, values.size() - 1))
	};
}

AstSemanticValue AstReductionBuilder::BuildReturnMethod(std::vector<AstSemanticValue> values)
{
	if (values.size() != 10 && values.size() != 11)
	{
		throw std::logic_error("Returning method declaration reduction expects 10 or 11 semantic values.");
	}

	return NodeValue{
		std::make_unique<FunctionDeclarationASTNode>(
			TakeParameterList(values, 2).front(),
			TakeToken(values, 4).lexeme,
			TakeParameterList(values, 6),
			TakeType(values, values.size() - 2),
			TakeNode(values, values.size() - 1))
	};
}

AstSemanticValue AstReductionBuilder::PassNode(
	std::vector<AstSemanticValue> values,
	const std::size_t index)
{
	if (index >= values.size())
	{
		throw std::logic_error("PassNode index is out of range.");
	}

	auto* node = std::get_if<NodeValue>(&values[index]);

	if (!node || !node->value)
	{
		throw std::logic_error("PassNode expects node semantic value.");
	}

	return NodeValue{ std::move(node->value) };
}

AstSemanticValue AstReductionBuilder::PassToken(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Pass token reduction");

	const auto* token = std::get_if<TokenValue>(&values[0]);

	if (!token)
	{
		throw std::logic_error("PassToken expects token semantic value.");
	}

	return TokenValue{ token->value };
}

std::unique_ptr<ProgramASTNode> AstReductionBuilder::TakeProgram(AstSemanticValue& value)
{
	auto* program = std::get_if<ProgramValue>(&value);

	if (!program || !program->value)
	{
		throw std::runtime_error("Expected program semantic value.");
	}

	return std::move(program->value);
}

ASTNodePtr AstReductionBuilder::TakeNode(std::vector<AstSemanticValue>& values, const std::size_t index)
{
	auto* node = std::get_if<NodeValue>(&values[index]);

	if (!node || !node->value)
	{
		throw std::runtime_error("Expected node semantic value.");
	}

	return std::move(node->value);
}

std::vector<ASTNodePtr> AstReductionBuilder::TakeStatementList(std::vector<AstSemanticValue>& values, const std::size_t index)
{
	auto* statementList = std::get_if<StatementListValue>(&values[index]);

	if (!statementList)
	{
		throw std::runtime_error("Expected statement list semantic value.");
	}

	return std::move(statementList->values);
}

std::vector<ASTNodePtr> AstReductionBuilder::TakeExpressionList(
	std::vector<AstSemanticValue>& values,
	const std::size_t index)
{
	auto* expressionList = std::get_if<ExpressionListValue>(&values[index]);

	if (!expressionList)
	{
		throw std::runtime_error("Expected expression list semantic value.");
	}

	return std::move(expressionList->values);
}

std::vector<std::string> AstReductionBuilder::TakeIdentifierList(
	std::vector<AstSemanticValue>& values,
	const std::size_t index)
{
	auto* identifierList = std::get_if<IdentifierListValue>(&values[index]);

	if (!identifierList)
	{
		throw std::runtime_error("Expected identifier list semantic value.");
	}

	return std::move(identifierList->values);
}

std::vector<std::string> AstReductionBuilder::TakeIdentifierNamesFromTargets(
	std::vector<AstSemanticValue>& values)
{
	const std::vector<ASTNodePtr> targets = TakeExpressionList(values, 0);
	std::vector<std::string> names;
	names.reserve(targets.size());
	for (const ASTNodePtr& target : targets)
	{
		const auto* identifier = dynamic_cast<const IdentifierASTNode*>(target.get());
		if (!identifier)
		{
			throw std::runtime_error("Short variable declaration target must be identifier.");
		}
		names.push_back(identifier->GetName());
	}
	return names;
}

std::vector<FunctionParameter> AstReductionBuilder::TakeParameterList(
	std::vector<AstSemanticValue>& values,
	const std::size_t index)
{
	auto* parameterList = std::get_if<ParameterListValue>(&values[index]);

	if (!parameterList)
	{
		throw std::runtime_error("Expected parameter list semantic value.");
	}

	return std::move(parameterList->values);
}

std::vector<TypeDescriptor> AstReductionBuilder::TakeTypeList(
	std::vector<AstSemanticValue>& values,
	const std::size_t index)
{
	if (auto* typeList = std::get_if<TypeListValue>(&values[index]))
	{
		return std::move(typeList->values);
	}

	if (auto* type = std::get_if<TypeValue>(&values[index]))
	{
		std::vector<TypeDescriptor> types;
		types.push_back(std::move(type->value));
		return types;
	}

	throw std::runtime_error("Expected type list semantic value.");
}

std::vector<StructField> AstReductionBuilder::TakeStructFieldList(
	std::vector<AstSemanticValue>& values,
	const std::size_t index)
{
	auto* fieldList = std::get_if<StructFieldListValue>(&values[index]);

	if (!fieldList)
	{
		throw std::runtime_error("Expected struct field list semantic value.");
	}

	return std::move(fieldList->values);
}

std::vector<StructFieldInitializer> AstReductionBuilder::TakeFieldInitializerList(
	std::vector<AstSemanticValue>& values,
	const std::size_t index)
{
	if (auto* initializerList = std::get_if<FieldInitializerListValue>(&values[index]))
	{
		return std::move(initializerList->values);
	}

	if (auto* structLiteralTail = std::get_if<StructLiteralTailValue>(&values[index]))
	{
		return std::move(structLiteralTail->fieldInitializers);
	}

	throw std::runtime_error("Expected struct field initializer list semantic value.");
}

TypeDescriptor AstReductionBuilder::TakeType(const std::vector<AstSemanticValue>& values, const std::size_t index)
{
	const auto* type = std::get_if<TypeValue>(&values[index]);

	if (!type)
	{
		throw std::runtime_error("Expected type semantic value.");
	}

	return type->value;
}

Token AstReductionBuilder::TakeToken(const std::vector<AstSemanticValue>& values, const std::size_t index)
{
	const auto* token = std::get_if<TokenValue>(&values[index]);

	if (!token)
	{
		throw std::runtime_error("Expected token semantic value.");
	}

	return token->value;
}

BinaryOperator AstReductionBuilder::ToBinaryOperator(const TokenType type)
{
	switch (type)
	{
	case TokenType::OR_OR:
		return BinaryOperator::OR;
	case TokenType::AND_AND:
		return BinaryOperator::AND;
	case TokenType::PLUS:
		return BinaryOperator::ADD;
	case TokenType::MINUS:
		return BinaryOperator::SUBTRACT;
	case TokenType::STAR:
		return BinaryOperator::MULTIPLY;
	case TokenType::SLASH:
		return BinaryOperator::DIVIDE;
	case TokenType::PERCENT:
		return BinaryOperator::MODULO;
	case TokenType::LESS:
		return BinaryOperator::LESS;
	case TokenType::LESS_EQUAL:
		return BinaryOperator::LESS_EQUAL;
	case TokenType::BANG_EQUAL:
		return BinaryOperator::NOT_EQUAL;
	case TokenType::EQUAL_EQUAL:
		return BinaryOperator::EQUAL;
	case TokenType::GREATER:
		return BinaryOperator::GREATER;
	case TokenType::GREATER_EQUAL:
		return BinaryOperator::GREATER_EQUAL;
	default:
		throw std::runtime_error("Unsupported binary operator token.");
	}
}

UnaryOperator AstReductionBuilder::ToUnaryOperator(const TokenType type)
{
	switch (type)
	{
	case TokenType::MINUS:
		return UnaryOperator::NEGATE;
	case TokenType::BANG:
		return UnaryOperator::NOT;
	default:
		throw std::runtime_error("Unsupported unary operator token.");
	}
}
