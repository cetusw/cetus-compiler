#include "ExpressionFormatter.h"

#include <sstream>

namespace
{
std::string FormatUnaryOperator(const UnaryOperator op)
{
	switch (op)
	{
	case UnaryOperator::NEGATE:
		return "-";
	case UnaryOperator::NOT:
		return "!";
	}
	return "?";
}

std::string FormatBinaryOperator(const BinaryOperator op)
{
	switch (op)
	{
	case BinaryOperator::OR:
		return "||";
	case BinaryOperator::AND:
		return "&&";
	case BinaryOperator::ADD:
		return "+";
	case BinaryOperator::SUBTRACT:
		return "-";
	case BinaryOperator::MULTIPLY:
		return "*";
	case BinaryOperator::DIVIDE:
		return "/";
	case BinaryOperator::MODULO:
		return "%";
	case BinaryOperator::LESS:
		return "<";
	case BinaryOperator::LESS_EQUAL:
		return "<=";
	case BinaryOperator::NOT_EQUAL:
		return "!=";
	case BinaryOperator::EQUAL:
		return "==";
	case BinaryOperator::GREATER:
		return ">";
	case BinaryOperator::GREATER_EQUAL:
		return ">=";
	}
	return "?";
}

std::string FormatExpressionList(const std::vector<ASTNodePtr>& expressions)
{
	std::ostringstream stream;
	for (std::size_t index = 0; index < expressions.size(); ++index)
	{
		if (index > 0)
		{
			stream << ", ";
		}
		stream << ExpressionFormatter::Format(*expressions[index]);
	}
	return stream.str();
}

std::string FormatFieldInitializers(const std::vector<StructFieldInitializer>& initializers)
{
	std::ostringstream stream;
	for (std::size_t index = 0; index < initializers.size(); ++index)
	{
		if (index > 0)
		{
			stream << ", ";
		}
		stream << initializers[index].name << ": "
			<< ExpressionFormatter::Format(*initializers[index].expression);
	}
	return stream.str();
}
} // namespace

std::string ExpressionFormatter::Format(const ASTNode& node)
{
	if (const auto* boolLiteral = dynamic_cast<const BoolLiteralASTNode*>(&node))
	{
		return boolLiteral->GetValue() ? "true" : "false";
	}
	if (dynamic_cast<const NilLiteralASTNode*>(&node))
	{
		return "nil";
	}
	if (const auto* intLiteral = dynamic_cast<const IntLiteralASTNode*>(&node))
	{
		return intLiteral->GetValue();
	}
	if (const auto* floatLiteral = dynamic_cast<const FloatLiteralASTNode*>(&node))
	{
		return floatLiteral->GetValue();
	}
	if (const auto* stringLiteral = dynamic_cast<const StringLiteralASTNode*>(&node))
	{
		return "\"" + stringLiteral->GetValue() + "\"";
	}
	if (const auto* arrayLiteral = dynamic_cast<const ArrayLiteralASTNode*>(&node))
	{
		return arrayLiteral->GetType().ToString() + "{"
			+ FormatExpressionList(arrayLiteral->GetElements()) + "}";
	}
	if (const auto* structLiteral = dynamic_cast<const StructLiteralASTNode*>(&node))
	{
		return structLiteral->GetTypeName() + "{"
			+ FormatFieldInitializers(structLiteral->GetInitializers()) + "}";
	}
	if (const auto* identifier = dynamic_cast<const IdentifierASTNode*>(&node))
	{
		return identifier->GetName();
	}
	if (const auto* addressOf = dynamic_cast<const AddressOfASTNode*>(&node))
	{
		return "&" + Format(addressOf->GetTarget());
	}
	if (const auto* unary = dynamic_cast<const UnaryASTNode*>(&node))
	{
		return FormatUnaryOperator(unary->GetOperator()) + Format(unary->GetOperand());
	}
	if (const auto* binary = dynamic_cast<const BinaryASTNode*>(&node))
	{
		return Format(binary->GetLeft()) + " " + FormatBinaryOperator(binary->GetOperator()) + " "
			+ Format(binary->GetRight());
	}
	if (const auto* memberAccess = dynamic_cast<const MemberAccessASTNode*>(&node))
	{
		return Format(memberAccess->GetObject()) + "." + memberAccess->GetMember();
	}
	if (const auto* index = dynamic_cast<const IndexASTNode*>(&node))
	{
		return Format(index->GetObject()) + "[" + Format(index->GetIndex()) + "]";
	}
	if (const auto* slice = dynamic_cast<const SliceExpressionASTNode*>(&node))
	{
		std::string text = Format(slice->GetObject()) + "[";
		if (slice->HasStart())
		{
			text += Format(*slice->GetStart());
		}
		text += ":";
		if (slice->HasEnd())
		{
			text += Format(*slice->GetEnd());
		}
		text += "]";
		return text;
	}
	if (const auto* call = dynamic_cast<const CallExpressionASTNode*>(&node))
	{
		std::string text;
		if (call->IsMethodCall())
		{
			text = Format(*call->GetReceiver()) + ".";
		}
		text += call->GetCalleeName() + "(" + FormatExpressionList(call->GetArguments()) + ")";
		return text;
	}

	return "<expr>";
}
