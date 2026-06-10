#include "AstReductionBuilder.h"

#include <stdexcept>

AstSemanticValue AstReductionBuilder::Build(const ParserRule& rule, std::vector<AstSemanticValue> values)
{
	switch (rule.semanticTag)
	{
	case SemanticTag::PASS_EXPR:
		return PassNode(std::move(values), 0);
	case SemanticTag::PASS_TOKEN:
		return PassToken(std::move(values), 0);
	case SemanticTag::GROUP:
		return PassNode(std::move(values), 1);
	case SemanticTag::UNARY:
		return BuildUnary(std::move(values));
	case SemanticTag::BINARY:
		return BuildBinary(std::move(values));
	case SemanticTag::BOOL_LITERAL:
		return BuildBoolLiteral(values);
	case SemanticTag::INT_LITERAL:
		return BuildIntLiteral(values);
	case SemanticTag::FLOAT_LITERAL:
		return BuildFloatLiteral(values);
	case SemanticTag::STRING_LITERAL:
		return BuildStringLiteral(values);
	case SemanticTag::IDENTIFIER:
		return BuildIdentifier(values);
	case SemanticTag::ADDRESS_OF:
		return BuildAddressOf(std::move(values));
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
	case SemanticTag::MEMBER_ACCESS:
		return BuildMemberAccess(std::move(values));
	case SemanticTag::INDEX_ACCESS:
		return BuildIndexAccess(std::move(values));
	case SemanticTag::ASSIGNMENT:
		return BuildAssignment(std::move(values));
	case SemanticTag::SHORT_VAR_DECLARATION:
		return BuildShortVariableDeclaration(std::move(values));
	case SemanticTag::VAR_INFERRED_DECLARATION:
		return BuildVarInferredDeclaration(std::move(values));
	case SemanticTag::VAR_TYPED_DECLARATION:
		return BuildVarTypedDeclaration(std::move(values));
	case SemanticTag::VAR_TYPED_INITIALIZED_DECLARATION:
		return BuildVarTypedInitializedDeclaration(std::move(values));
	case SemanticTag::EXPRESSION_STATEMENT:
		return BuildExpressionStatement(std::move(values));
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
	case SemanticTag::FUNCTION_VOID_NO_PARAMS:
		return BuildVoidFunctionNoParams(std::move(values));
	case SemanticTag::FUNCTION_RETURN_NO_PARAMS:
		return BuildReturnFunctionNoParams(std::move(values));
	case SemanticTag::FUNCTION_VOID:
		return BuildVoidFunction(std::move(values));
	case SemanticTag::FUNCTION_RETURN:
		return BuildReturnFunction(std::move(values));
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
	return {
		std::make_unique<BinaryASTNode>(
			TakeNode(values, 0),
			ToBinaryOperator(TakeToken(values, 1).type),
			TakeNode(values, 2)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildUnary(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 2, "Unary reduction");
	return {
		std::make_unique<UnaryASTNode>(
			ToUnaryOperator(TakeToken(values, 0).type),
			TakeNode(values, 1)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildBoolLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Bool literal reduction");
	// TODO сделать что-то с такими длинными объявлениями
	return { std::make_unique<BoolLiteralASTNode>(TakeToken(values, 0).type == TokenType::TRUE), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildIntLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Int literal reduction");
	return { std::make_unique<IntLiteralASTNode>(TakeToken(values, 0).lexeme), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildFloatLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Float literal reduction");
	return { std::make_unique<FloatLiteralASTNode>(TakeToken(values, 0).lexeme), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildStringLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "String literal reduction");
	return { std::make_unique<StringLiteralASTNode>(TakeToken(values, 0).lexeme), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildIdentifier(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Identifier reduction");
	return { std::make_unique<IdentifierASTNode>(TakeToken(values, 0).lexeme), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildIdentifierList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Identifier list reduction");
	std::vector<std::string> identifiers = TakeIdentifierList(values, 0);
	identifiers.push_back(TakeToken(values, 2).lexeme);
	return { nullptr, std::nullopt, std::move(identifiers) };
}

AstSemanticValue AstReductionBuilder::BuildSingleIdentifierList(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Single identifier list reduction");
	return { nullptr, std::nullopt, { TakeToken(values, 0).lexeme } };
}

AstSemanticValue AstReductionBuilder::BuildExpressionList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Expression list reduction");
	std::vector<ASTNodePtr> expressions = TakeExpressionList(values, 0);
	expressions.push_back(TakeNode(values, 2));
	return { nullptr, std::nullopt, {}, std::move(expressions) };
}

AstSemanticValue AstReductionBuilder::BuildSingleExpressionList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Single expression list reduction");
	std::vector<ASTNodePtr> expressions;
	expressions.push_back(TakeNode(values, 0));
	return { nullptr, std::nullopt, {}, std::move(expressions) };
}

AstSemanticValue AstReductionBuilder::BuildTypeName(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Type name reduction");
	const std::string& typeName = TakeToken(values, 0).lexeme;
	if (typeName == "int")
	{
		return { nullptr, std::nullopt, {}, {}, {}, Type::INT };
	}
	if (typeName == "float")
	{
		return { nullptr, std::nullopt, {}, {}, {}, Type::FLOAT };
	}
	if (typeName == "bool")
	{
		return { nullptr, std::nullopt, {}, {}, {}, Type::BOOL };
	}
	if (typeName == "string")
	{
		return { nullptr, std::nullopt, {}, {}, {}, Type::STRING };
	}

	throw std::runtime_error("Unsupported type name: " + typeName);
}

AstSemanticValue AstReductionBuilder::BuildParameter(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 2, "Parameter reduction");
	return { nullptr, std::nullopt, {}, {}, { FunctionParameter{ TakeToken(values, 0).lexeme, TakeType(values, 1) } } };
}

AstSemanticValue AstReductionBuilder::BuildPointerParameter(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 3, "Pointer parameter reduction");
	return {
		nullptr,
		std::nullopt,
		{},
		{},
		{ FunctionParameter{ TakeToken(values, 0).lexeme, TakeType(values, 2), true } }
	};
}

AstSemanticValue AstReductionBuilder::BuildAddressOf(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 2, "Address-of reduction");
	return { std::make_unique<AddressOfASTNode>(TakeNode(values, 1)), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildParameterList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Parameter list reduction");
	std::vector<FunctionParameter> parameters = TakeParameterList(values, 0);
	std::vector<FunctionParameter> nextParameter = TakeParameterList(values, 2);
	parameters.push_back(std::move(nextParameter.front()));
	return { nullptr, std::nullopt, {}, {}, std::move(parameters) };
}

AstSemanticValue AstReductionBuilder::BuildSingleParameterList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Single parameter list reduction");
	return { nullptr, std::nullopt, {}, {}, TakeParameterList(values, 0) };
}

AstSemanticValue AstReductionBuilder::BuildCallNoArgs(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 3, "Function call without arguments reduction");
	return { std::make_unique<CallExpressionASTNode>(TakeToken(values, 0).lexeme, std::vector<ASTNodePtr>{}), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildCall(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 4, "Function call reduction");
	return {
		std::make_unique<CallExpressionASTNode>(
			TakeToken(values, 0).lexeme,
			TakeExpressionList(values, 2)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildMemberAccess(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Member access reduction");
	return { std::make_unique<MemberAccessASTNode>(TakeNode(values, 0), TakeToken(values, 2).lexeme), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildIndexAccess(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 4, "Index access reduction");
	return { std::make_unique<IndexASTNode>(TakeNode(values, 0), TakeNode(values, 2)), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildAssignment(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Assignment reduction");
	return {
		std::make_unique<AssignmentASTNode>(
			TakeIdentifierList(values, 0),
			TakeExpressionList(values, 2)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildShortVariableDeclaration(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Short variable declaration reduction");
	return {
		std::make_unique<ShortVariableDeclarationASTNode>(
			TakeIdentifierList(values, 0),
			TakeExpressionList(values, 2)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildVarInferredDeclaration(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 4, "Inferred variable declaration reduction");
	return {
		std::make_unique<VariableDeclarationASTNode>(
			TakeIdentifierList(values, 1),
			std::nullopt,
			TakeExpressionList(values, 3)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildVarTypedDeclaration(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Typed variable declaration reduction");
	return {
		std::make_unique<VariableDeclarationASTNode>(
			TakeIdentifierList(values, 1),
			TakeType(values, 2),
			std::vector<ASTNodePtr>{}),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildVarTypedInitializedDeclaration(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 5, "Typed initialized variable declaration reduction");
	return {
		std::make_unique<VariableDeclarationASTNode>(
			TakeIdentifierList(values, 1),
			TakeType(values, 2),
			TakeExpressionList(values, 4)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildExpressionStatement(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Expression statement reduction");
	return { std::make_unique<ExpressionStatementASTNode>(TakeNode(values, 0)), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildProgram(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Program reduction");
	return { std::make_unique<ProgramASTNode>(TakeNode(values, 0)), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildEmptyProgram(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 0, "Empty program reduction");
	return {
		std::make_unique<ProgramASTNode>(
			std::make_unique<StatementListASTNode>(std::vector<ASTNodePtr>{})),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildStatementList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 2, "Statement list reduction");

	const ASTNodePtr listNode = TakeNode(values, 0);
	// TODO избавиться от dynamic_cast
	auto* statementList = dynamic_cast<StatementListASTNode*>(listNode.get());
	if (!statementList)
	{
		throw std::logic_error("Statement list reduction expects StatementListASTNode as left operand.");
	}

	std::vector<ASTNodePtr> statements = statementList->TakeStatements();
	statements.push_back(TakeNode(values, 1));
	return { std::make_unique<StatementListASTNode>(std::move(statements)), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildSingleStatementList(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 1, "Single statement list reduction");

	std::vector<ASTNodePtr> statements;
	statements.push_back(TakeNode(values, 0));
	return { std::make_unique<StatementListASTNode>(std::move(statements)), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildBlock(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Block reduction");
	return { std::make_unique<BlockASTNode>(TakeNode(values, 1)), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildEmptyBlock(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 2, "Empty block reduction");
	return {
		std::make_unique<BlockASTNode>(
			std::make_unique<StatementListASTNode>(std::vector<ASTNodePtr>{})),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildIf(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "If reduction");
	return {
		std::make_unique<IfASTNode>(
			TakeNode(values, 1),
			TakeNode(values, 2)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildIfElse(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 5, "If/else reduction");
	return {
		std::make_unique<IfASTNode>(
			TakeNode(values, 1),
			TakeNode(values, 2),
			TakeNode(values, 4)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildForCondition(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "For condition reduction");
	return {
		std::make_unique<ForASTNode>(
			nullptr,
			TakeNode(values, 1),
			nullptr,
			TakeNode(values, 2)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildForClassic(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 7, "Classic for reduction");
	return {
		std::make_unique<ForASTNode>(
			TakeNode(values, 1),
			TakeNode(values, 3),
			TakeNode(values, 5),
			TakeNode(values, 6)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildBreak(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 2, "Break reduction");
	return { std::make_unique<BreakASTNode>(), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildContinue(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 2, "Continue reduction");
	return { std::make_unique<ContinueASTNode>(), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildReturnVoid(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 2, "Return void reduction");
	return { std::make_unique<ReturnASTNode>(), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildReturnValue(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Return value reduction");
	return { std::make_unique<ReturnASTNode>(TakeNode(values, 1)), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildVoidFunctionNoParams(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 5, "Void function declaration reduction");
	return {
		std::make_unique<FunctionDeclarationASTNode>(
			TakeToken(values, 1).lexeme,
			std::vector<FunctionParameter>{},
			std::nullopt,
			TakeNode(values, 4)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildReturnFunctionNoParams(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 6, "Returning function declaration reduction");
	return {
		std::make_unique<FunctionDeclarationASTNode>(
			TakeToken(values, 1).lexeme,
			std::vector<FunctionParameter>{},
			TakeType(values, 4),
			TakeNode(values, 5)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildVoidFunction(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 6, "Void function declaration reduction");
	return {
		std::make_unique<FunctionDeclarationASTNode>(
			TakeToken(values, 1).lexeme,
			TakeParameterList(values, 3),
			std::nullopt,
			TakeNode(values, 5)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildReturnFunction(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 7, "Returning function declaration reduction");
	return {
		std::make_unique<FunctionDeclarationASTNode>(
			TakeToken(values, 1).lexeme,
			TakeParameterList(values, 3),
			TakeType(values, 5),
			TakeNode(values, 6)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::PassNode(std::vector<AstSemanticValue> values, const std::size_t index)
{
	if (index >= values.size())
	{
		throw std::logic_error("PassNode index is out of range.");
	}
	if (!values[index].node)
	{
		throw std::logic_error("PassNode expects node semantic value.");
	}
	return { std::move(values[index].node), std::nullopt };
}

AstSemanticValue AstReductionBuilder::PassToken(std::vector<AstSemanticValue> values, const std::size_t index)
{
	if (index >= values.size())
	{
		throw std::logic_error("PassToken index is out of range.");
	}
	return { nullptr, values[index].token };
}

ASTNodePtr AstReductionBuilder::TakeNode(std::vector<AstSemanticValue>& values, const std::size_t index)
{
	if (!values[index].node)
	{
		throw std::runtime_error("Expected node semantic value.");
	}

	return std::move(values[index].node);
}

std::vector<ASTNodePtr> AstReductionBuilder::TakeExpressionList(
	std::vector<AstSemanticValue>& values,
	const std::size_t index)
{
	if (values[index].expressions.empty())
	{
		throw std::runtime_error("Expected expression list semantic value.");
	}

	return std::move(values[index].expressions);
}

std::vector<std::string> AstReductionBuilder::TakeIdentifierList(
	std::vector<AstSemanticValue>& values,
	const std::size_t index)
{
	if (values[index].identifiers.empty())
	{
		throw std::runtime_error("Expected identifier list semantic value.");
	}

	return std::move(values[index].identifiers);
}

std::vector<FunctionParameter> AstReductionBuilder::TakeParameterList(
	std::vector<AstSemanticValue>& values,
	const std::size_t index)
{
	if (values[index].parameters.empty())
	{
		throw std::runtime_error("Expected parameter list semantic value.");
	}

	return std::move(values[index].parameters);
}

Type AstReductionBuilder::TakeType(const std::vector<AstSemanticValue>& values, const std::size_t index)
{
	if (!values[index].type.has_value())
	{
		throw std::runtime_error("Expected type semantic value.");
	}

	return *values[index].type;
}

Token AstReductionBuilder::TakeToken(const std::vector<AstSemanticValue>& values, const std::size_t index)
{
	if (!values[index].token.has_value())
	{
		throw std::runtime_error("Expected token semantic value.");
	}

	return *values[index].token;
}

BinaryOperator AstReductionBuilder::ToBinaryOperator(const TokenType type)
{
	switch (type)
	{
	case TokenType::OR_OR: return BinaryOperator::OR;
	case TokenType::AND_AND: return BinaryOperator::AND;
	case TokenType::PLUS: return BinaryOperator::ADD;
	case TokenType::MINUS: return BinaryOperator::SUBTRACT;
	case TokenType::STAR: return BinaryOperator::MULTIPLY;
	case TokenType::SLASH: return BinaryOperator::DIVIDE;
	case TokenType::PERCENT: return BinaryOperator::MODULO;
	case TokenType::LESS: return BinaryOperator::LESS;
	case TokenType::LESS_EQUAL: return BinaryOperator::LESS_EQUAL;
	case TokenType::BANG_EQUAL: return BinaryOperator::NOT_EQUAL;
	case TokenType::EQUAL_EQUAL: return BinaryOperator::EQUAL;
	case TokenType::GREATER: return BinaryOperator::GREATER;
	case TokenType::GREATER_EQUAL: return BinaryOperator::GREATER_EQUAL;
	default: throw std::runtime_error("Unsupported binary operator token.");
	}
}

UnaryOperator AstReductionBuilder::ToUnaryOperator(const TokenType type)
{
	switch (type)
	{
	case TokenType::MINUS: return UnaryOperator::NEGATE;
	case TokenType::BANG: return UnaryOperator::NOT;
	default: throw std::runtime_error("Unsupported unary operator token.");
	}
}
