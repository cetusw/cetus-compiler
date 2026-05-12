#include "AstReductionBuilder.h"

#include <stdexcept>

AstSemanticValue AstReductionBuilder::Build(const ParserRule& rule, std::vector<AstSemanticValue> values)
{
	switch (rule.semanticTag)
	{
	case SemanticTag::PASS_EXPR:
		return PassExpr(std::move(values), 0);
	case SemanticTag::PASS_TOKEN:
		return PassToken(std::move(values), 0);
	case SemanticTag::GROUP:
		return PassExpr(std::move(values), 1);
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
	case SemanticTag::IDENTIFIER:
		return BuildIdentifier(values);
	case SemanticTag::MEMBER_ACCESS:
		return BuildMemberAccess(std::move(values));
	case SemanticTag::INDEX_ACCESS:
		return BuildIndexAccess(std::move(values));
	case SemanticTag::ASSIGNMENT:
		return BuildAssignment(std::move(values));
	case SemanticTag::SEQUENCE:
		return BuildSequence(std::move(values));
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
		std::make_unique<BinaryExpr>(
			TakeExpr(values, 0),
			ToBinaryOperator(TakeToken(values, 1).type),
			TakeExpr(values, 2)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildUnary(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 2, "Unary reduction");
	return {
		std::make_unique<UnaryExpr>(
			ToUnaryOperator(TakeToken(values, 0).type),
			TakeExpr(values, 1)),
		std::nullopt
	};
}

AstSemanticValue AstReductionBuilder::BuildBoolLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Bool literal reduction");
	// TODO сделать что-то с такими длинными объявлениями
	return { std::make_unique<BoolLiteralExpr>(TakeToken(values, 0).type == TokenType::TRUE), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildIntLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Int literal reduction");
	return { std::make_unique<IntLiteralExpr>(TakeToken(values, 0).lexeme), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildFloatLiteral(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Float literal reduction");
	return { std::make_unique<FloatLiteralExpr>(TakeToken(values, 0).lexeme), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildIdentifier(const std::vector<AstSemanticValue>& values)
{
	RequireValueCount(values, 1, "Identifier reduction");
	return { std::make_unique<IdentifierExpr>(TakeToken(values, 0).lexeme), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildMemberAccess(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Member access reduction");
	return { std::make_unique<MemberAccessExpr>(TakeExpr(values, 0), TakeToken(values, 2).lexeme), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildIndexAccess(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 4, "Index access reduction");
	return { std::make_unique<IndexExpr>(TakeExpr(values, 0), TakeExpr(values, 2)), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildAssignment(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Assignment reduction");
	return { std::make_unique<AssignmentExpr>(TakeToken(values, 0).lexeme, TakeExpr(values, 2)), std::nullopt };
}

AstSemanticValue AstReductionBuilder::BuildSequence(std::vector<AstSemanticValue> values)
{
	RequireValueCount(values, 3, "Sequence reduction");

	std::vector<ExprPtr> expressions;
	expressions.push_back(TakeExpr(values, 0));
	expressions.push_back(TakeExpr(values, 2));
	return { std::make_unique<SequenceExpr>(std::move(expressions)), std::nullopt };
}

AstSemanticValue AstReductionBuilder::PassExpr(std::vector<AstSemanticValue> values, const std::size_t index)
{
	if (index >= values.size())
	{
		throw std::logic_error("PassExpr index is out of range.");
	}
	if (!values[index].expr)
	{
		throw std::logic_error("PassExpr expects expression semantic value.");
	}
	return { std::move(values[index].expr), std::nullopt };
}

AstSemanticValue AstReductionBuilder::PassToken(std::vector<AstSemanticValue> values, const std::size_t index)
{
	if (index >= values.size())
	{
		throw std::logic_error("PassToken index is out of range.");
	}
	return { nullptr, values[index].token };
}

ExprPtr AstReductionBuilder::TakeExpr(std::vector<AstSemanticValue>& values, const std::size_t index)
{
	if (!values[index].expr)
	{
		throw std::runtime_error("Expected expression semantic value.");
	}

	return std::move(values[index].expr);
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
