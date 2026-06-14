#pragma once

#include "src/frontend/lexical/Token.h"
#include "src/frontend/syntax/ast/ASTNode.h"
#include "src/frontend/syntax/lalr/types/PreparedGrammar.h"
#include <optional>
#include <vector>

// TODO переделать на variant
struct AstSemanticValue
{
	ASTNodePtr node = nullptr;
	std::optional<Token> token = std::nullopt;
	std::vector<std::string> identifiers = {};
	std::vector<ASTNodePtr> expressions = {};
	std::vector<FunctionParameter> parameters = {};
	std::optional<TypeDescriptor> type = std::nullopt;
	std::vector<TypeDescriptor> types = {};
	std::vector<StructField> fields = {};
	std::vector<StructFieldInitializer> fieldInitializers = {};
	bool hasStructLiteralTail = false;
	bool compositeLiteralInitializer = false;
};

// TODO отрефакторить. слишком много методов
class AstReductionBuilder
{
public:
	[[nodiscard]] static AstSemanticValue Build(const ParserRule& rule, std::vector<AstSemanticValue> values);

private:
	static void RequireValueCount(const std::vector<AstSemanticValue>& values, std::size_t expectedCount, const char* actionName);
	[[nodiscard]] static AstSemanticValue BuildBinary(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildUnary(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildBoolLiteral(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildNilLiteral(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildIntLiteral(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildFloatLiteral(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildStringLiteral(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildArrayLiteral(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildEmptyArrayLiteral(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildStructLiteral(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildEmptyStructLiteral(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildIdentifier(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildNamedPostfix(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildIdentifierTail(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildStructLiteralTail(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildEmptyStructLiteralTail(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildAddressOf(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildAddressOfStructLiteral(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildAddressOfEmptyStructLiteral(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildIdentifierList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildSingleIdentifierList(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildExpressionList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildSingleExpressionList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildTypeName(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildArrayType(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildSliceType(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildPointerType(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildSingleReturnType(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildTupleReturnType(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildTypeList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildSingleTypeList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildParameter(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildPointerParameter(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildParameterList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildSingleParameterList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildCallNoArgs(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildCall(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildMethodCallNoArgs(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildMethodCall(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildMemberAccess(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildIndexAccess(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildSliceExpression(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildAssignableList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildSingleAssignableList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildAssignment(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildIncrement(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildDecrement(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildStructField(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildStructFieldList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildSingleStructFieldList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildFieldInitializer(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildFieldInitializerList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildSingleFieldInitializerList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildStructDeclaration(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildShortVariableDeclaration(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildVarInferredDeclaration(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildVarTypedDeclaration(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildVarTypedInitializedDeclaration(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildVarTypedDeclarationFull(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildVarTypedCompositeDeclaration(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildVarTypedEmptyCompositeDeclaration(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildExpressionStatement(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildAssertStatement(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildForAllStatement(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildEmptyStatement(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildProgram(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildEmptyProgram(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildStatementList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildSingleStatementList(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildBlock(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildEmptyBlock(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildIf(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildIfElse(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildForCondition(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildForClassic(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildBreak(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildContinue(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildReturnVoid(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildReturnValue(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildTestDeclaration(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildVoidFunctionNoParams(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildReturnFunctionNoParams(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildVoidFunction(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildReturnFunction(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildVoidMethodNoParams(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildReturnMethodNoParams(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildVoidMethod(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildReturnMethod(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue PassNode(std::vector<AstSemanticValue> values, std::size_t index);
	[[nodiscard]] static AstSemanticValue PassToken(std::vector<AstSemanticValue> values, std::size_t index);
	[[nodiscard]] static ASTNodePtr TakeNode(std::vector<AstSemanticValue>& values, std::size_t index);
	[[nodiscard]] static std::vector<ASTNodePtr> TakeExpressionList(std::vector<AstSemanticValue>& values, std::size_t index);
	[[nodiscard]] static std::vector<std::string> TakeIdentifierList(std::vector<AstSemanticValue>& values, std::size_t index);
	[[nodiscard]] static std::vector<std::string> TakeIdentifierNamesFromTargets(std::vector<AstSemanticValue>& values, std::size_t index);
	[[nodiscard]] static std::vector<FunctionParameter> TakeParameterList(std::vector<AstSemanticValue>& values, std::size_t index);
	[[nodiscard]] static std::vector<TypeDescriptor> TakeTypeList(std::vector<AstSemanticValue>& values, std::size_t index);
	[[nodiscard]] static std::vector<StructField> TakeStructFieldList(std::vector<AstSemanticValue>& values, std::size_t index);
	[[nodiscard]] static std::vector<StructFieldInitializer> TakeFieldInitializerList(std::vector<AstSemanticValue>& values, std::size_t index);
	[[nodiscard]] static TypeDescriptor TakeType(const std::vector<AstSemanticValue>& values, std::size_t index);
	[[nodiscard]] static Token TakeToken(const std::vector<AstSemanticValue>& values, std::size_t index);
	[[nodiscard]] static BinaryOperator ToBinaryOperator(TokenType type);
	[[nodiscard]] static UnaryOperator ToUnaryOperator(TokenType type);
};
