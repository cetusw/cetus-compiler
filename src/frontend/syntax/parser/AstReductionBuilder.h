#pragma once

#include "src/frontend/lexer/Token.h"
#include "src/frontend/syntax/ast/Expr.h"
#include "src/frontend/syntax/lalr/types/ParserDefinition.h"
#include <optional>
#include <vector>

struct AstSemanticValue
{
	ExprPtr expr;
	std::optional<Token> token;
};

class AstReductionBuilder
{
public:
	[[nodiscard]] static AstSemanticValue Build(const ParserRule& rule, std::vector<AstSemanticValue> values);

private:
	static void RequireValueCount(const std::vector<AstSemanticValue>& values, std::size_t expectedCount, const char* actionName);
	[[nodiscard]] static AstSemanticValue BuildBinary(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildUnary(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildBoolLiteral(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildIntLiteral(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildFloatLiteral(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildIdentifier(const std::vector<AstSemanticValue>& values);
	[[nodiscard]] static AstSemanticValue BuildMemberAccess(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue BuildIndexAccess(std::vector<AstSemanticValue> values);
	[[nodiscard]] static AstSemanticValue PassExpr(std::vector<AstSemanticValue> values, std::size_t index);
	[[nodiscard]] static AstSemanticValue PassToken(std::vector<AstSemanticValue> values, std::size_t index);
	[[nodiscard]] static ExprPtr TakeExpr(std::vector<AstSemanticValue>& values, std::size_t index);
	[[nodiscard]] static Token TakeToken(const std::vector<AstSemanticValue>& values, std::size_t index);
	[[nodiscard]] static BinaryOperator ToBinaryOperator(TokenType type);
	[[nodiscard]] static UnaryOperator ToUnaryOperator(TokenType type);
};
