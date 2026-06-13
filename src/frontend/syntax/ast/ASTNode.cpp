#include "ASTNode.h"

#include <sstream>
#include <utility>

std::string DescribeExpression(const ASTNode& node);

namespace
{
std::string DescribeUnaryOperator(const UnaryOperator op)
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

std::string DescribeBinaryOperator(const BinaryOperator op)
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

std::string DescribeFieldInitializers(const std::vector<StructFieldInitializer>& initializers)
{
	std::ostringstream stream;
	for (std::size_t index = 0; index < initializers.size(); ++index)
	{
		if (index > 0)
		{
			stream << ", ";
		}
		stream << initializers[index].name << ": " << DescribeExpression(*initializers[index].expression);
	}
	return stream.str();
}

std::string DescribeExpressionList(const std::vector<ASTNodePtr>& expressions)
{
	std::ostringstream stream;
	for (std::size_t index = 0; index < expressions.size(); ++index)
	{
		if (index > 0)
		{
			stream << ", ";
		}
		stream << DescribeExpression(*expressions[index]);
	}
	return stream.str();
}
} // namespace

std::optional<TypeDescriptor> ASTNode::GetInferredType() const
{
	return m_inferredType;
}

void ASTNode::SetInferredType(TypeDescriptor type) const
{
	m_inferredType = std::move(type);
}

// TODO вынести формирование сообщения об ошибке в одно место (в то самое пхпхпхпхп)
std::string DescribeExpression(const ASTNode& node)
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
		return arrayLiteral->GetType().ToString() + "{" + DescribeExpressionList(arrayLiteral->GetElements()) + "}";
	}
	if (const auto* structLiteral = dynamic_cast<const StructLiteralASTNode*>(&node))
	{
		return structLiteral->GetTypeName() + "{" + DescribeFieldInitializers(structLiteral->GetInitializers()) + "}";
	}
	if (const auto* identifier = dynamic_cast<const IdentifierASTNode*>(&node))
	{
		return identifier->GetName();
	}
	if (const auto* addressOf = dynamic_cast<const AddressOfASTNode*>(&node))
	{
		return "&" + DescribeExpression(addressOf->GetTarget());
	}
	if (const auto* unary = dynamic_cast<const UnaryASTNode*>(&node))
	{
		return DescribeUnaryOperator(unary->GetOperator()) + DescribeExpression(unary->GetOperand());
	}
	if (const auto* binary = dynamic_cast<const BinaryASTNode*>(&node))
	{
		return DescribeExpression(binary->GetLeft()) + " " + DescribeBinaryOperator(binary->GetOperator()) + " "
			+ DescribeExpression(binary->GetRight());
	}
	if (const auto* memberAccess = dynamic_cast<const MemberAccessASTNode*>(&node))
	{
		return DescribeExpression(memberAccess->GetObject()) + "." + memberAccess->GetMember();
	}
	if (const auto* index = dynamic_cast<const IndexASTNode*>(&node))
	{
		return DescribeExpression(index->GetObject()) + "[" + DescribeExpression(index->GetIndex()) + "]";
	}
	if (const auto* slice = dynamic_cast<const SliceExpressionASTNode*>(&node))
	{
		std::string text = DescribeExpression(slice->GetObject()) + "[";
		if (slice->HasStart())
		{
			text += DescribeExpression(*slice->GetStart());
		}
		text += ":";
		if (slice->HasEnd())
		{
			text += DescribeExpression(*slice->GetEnd());
		}
		text += "]";
		return text;
	}
	if (const auto* call = dynamic_cast<const CallExpressionASTNode*>(&node))
	{
		std::string text;
		if (call->IsMethodCall())
		{
			text = DescribeExpression(*call->GetReceiver()) + ".";
		}
		text += call->GetCalleeName() + "(" + DescribeExpressionList(call->GetArguments()) + ")";
		return text;
	}

	return "<expr>";
}

BoolLiteralASTNode::BoolLiteralASTNode(const bool value)
	: m_value(value)
{
}

bool BoolLiteralASTNode::GetValue() const
{
	return m_value;
}

void BoolLiteralASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

void NilLiteralASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

IntLiteralASTNode::IntLiteralASTNode(std::string value)
	: m_value(std::move(value))
{
}

const std::string& IntLiteralASTNode::GetValue() const
{
	return m_value;
}

void IntLiteralASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

FloatLiteralASTNode::FloatLiteralASTNode(std::string value)
	: m_value(std::move(value))
{
}

const std::string& FloatLiteralASTNode::GetValue() const
{
	return m_value;
}

void FloatLiteralASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

StringLiteralASTNode::StringLiteralASTNode(std::string value)
	: m_value(std::move(value))
{
}

const std::string& StringLiteralASTNode::GetValue() const
{
	return m_value;
}

void StringLiteralASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

ArrayLiteralASTNode::ArrayLiteralASTNode(TypeDescriptor type, std::vector<ASTNodePtr> elements)
	: m_type(std::move(type))
	, m_elements(std::move(elements))
{
}

const TypeDescriptor& ArrayLiteralASTNode::GetType() const
{
	return m_type;
}

const std::vector<ASTNodePtr>& ArrayLiteralASTNode::GetElements() const
{
	return m_elements;
}

void ArrayLiteralASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

StructLiteralASTNode::StructLiteralASTNode(std::string typeName, std::vector<StructFieldInitializer> initializers)
	: m_typeName(std::move(typeName))
	, m_initializers(std::move(initializers))
{
}

const std::string& StructLiteralASTNode::GetTypeName() const
{
	return m_typeName;
}

const std::vector<StructFieldInitializer>& StructLiteralASTNode::GetInitializers() const
{
	return m_initializers;
}

void StructLiteralASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

IdentifierASTNode::IdentifierASTNode(std::string name)
	: m_name(std::move(name))
{
}

const std::string& IdentifierASTNode::GetName() const
{
	return m_name;
}

void IdentifierASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

AddressOfASTNode::AddressOfASTNode(ASTNodePtr target)
	: m_target(std::move(target))
{
}

const ASTNode& AddressOfASTNode::GetTarget() const
{
	return *m_target;
}

void AddressOfASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

UnaryASTNode::UnaryASTNode(const UnaryOperator op, ASTNodePtr operand)
	: m_op(op)
	, m_operand(std::move(operand))
{
}

UnaryOperator UnaryASTNode::GetOperator() const
{
	return m_op;
}

const ASTNode& UnaryASTNode::GetOperand() const
{
	return *m_operand;
}

void UnaryASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

BinaryASTNode::BinaryASTNode(ASTNodePtr left, const BinaryOperator op, ASTNodePtr right)
	: m_left(std::move(left))
	, m_op(op)
	, m_right(std::move(right))
{
}

const ASTNode& BinaryASTNode::GetLeft() const
{
	return *m_left;
}

BinaryOperator BinaryASTNode::GetOperator() const
{
	return m_op;
}

const ASTNode& BinaryASTNode::GetRight() const
{
	return *m_right;
}

void BinaryASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

MemberAccessASTNode::MemberAccessASTNode(ASTNodePtr object, std::string member)
	: m_object(std::move(object))
	, m_member(std::move(member))
{
}

const ASTNode& MemberAccessASTNode::GetObject() const
{
	return *m_object;
}

const std::string& MemberAccessASTNode::GetMember() const
{
	return m_member;
}

void MemberAccessASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

IndexASTNode::IndexASTNode(ASTNodePtr object, ASTNodePtr index)
	: m_object(std::move(object))
	, m_index(std::move(index))
{
}

const ASTNode& IndexASTNode::GetObject() const
{
	return *m_object;
}

const ASTNode& IndexASTNode::GetIndex() const
{
	return *m_index;
}

void IndexASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

SliceExpressionASTNode::SliceExpressionASTNode(ASTNodePtr object, ASTNodePtr start, ASTNodePtr end)
	: m_object(std::move(object))
	, m_start(std::move(start))
	, m_end(std::move(end))
{
}

const ASTNode& SliceExpressionASTNode::GetObject() const
{
	return *m_object;
}

const ASTNode* SliceExpressionASTNode::GetStart() const
{
	return m_start.get();
}

const ASTNode* SliceExpressionASTNode::GetEnd() const
{
	return m_end.get();
}

bool SliceExpressionASTNode::HasStart() const
{
	return m_start != nullptr;
}

bool SliceExpressionASTNode::HasEnd() const
{
	return m_end != nullptr;
}

void SliceExpressionASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

CallExpressionASTNode::CallExpressionASTNode(std::string calleeName, std::vector<ASTNodePtr> arguments)
	: m_receiver(nullptr)
	, m_calleeName(std::move(calleeName))
	, m_arguments(std::move(arguments))
{
}

CallExpressionASTNode::CallExpressionASTNode(ASTNodePtr receiver, std::string calleeName, std::vector<ASTNodePtr> arguments)
	: m_receiver(std::move(receiver))
	, m_calleeName(std::move(calleeName))
	, m_arguments(std::move(arguments))
{
}

const std::string& CallExpressionASTNode::GetCalleeName() const
{
	return m_calleeName;
}

bool CallExpressionASTNode::IsMethodCall() const
{
	return m_receiver != nullptr;
}

const ASTNode* CallExpressionASTNode::GetReceiver() const
{
	return m_receiver.get();
}

const std::vector<ASTNodePtr>& CallExpressionASTNode::GetArguments() const
{
	return m_arguments;
}

void CallExpressionASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

AssignmentASTNode::AssignmentASTNode(std::vector<ASTNodePtr> targets, std::vector<ASTNodePtr> values)
	: m_targets(std::move(targets))
	, m_values(std::move(values))
{
}

const std::vector<ASTNodePtr>& AssignmentASTNode::GetTargets() const
{
	return m_targets;
}

const std::vector<ASTNodePtr>& AssignmentASTNode::GetValues() const
{
	return m_values;
}

void AssignmentASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

IncrementASTNode::IncrementASTNode(ASTNodePtr target)
	: m_target(std::move(target))
{
}

const ASTNode& IncrementASTNode::GetTarget() const
{
	return *m_target;
}

void IncrementASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

DecrementASTNode::DecrementASTNode(ASTNodePtr target)
	: m_target(std::move(target))
{
}

const ASTNode& DecrementASTNode::GetTarget() const
{
	return *m_target;
}

void DecrementASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

ShortVariableDeclarationASTNode::ShortVariableDeclarationASTNode(std::vector<std::string> names, std::vector<ASTNodePtr> values)
	: m_names(std::move(names))
	, m_values(std::move(values))
{
}

const std::vector<std::string>& ShortVariableDeclarationASTNode::GetNames() const
{
	return m_names;
}

const std::vector<ASTNodePtr>& ShortVariableDeclarationASTNode::GetValues() const
{
	return m_values;
}

void ShortVariableDeclarationASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

VariableDeclarationASTNode::VariableDeclarationASTNode(
	std::vector<std::string> names,
	std::optional<TypeDescriptor> declaredType,
	std::vector<ASTNodePtr> values)
	: m_names(std::move(names))
	, m_declaredType(std::move(declaredType))
	, m_values(std::move(values))
{
}

const std::vector<std::string>& VariableDeclarationASTNode::GetNames() const
{
	return m_names;
}

std::optional<TypeDescriptor> VariableDeclarationASTNode::GetDeclaredType() const
{
	return m_declaredType;
}

const std::vector<ASTNodePtr>& VariableDeclarationASTNode::GetValues() const
{
	return m_values;
}

void VariableDeclarationASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

ExpressionStatementASTNode::ExpressionStatementASTNode(ASTNodePtr expression)
	: m_expression(std::move(expression))
{
}

const ASTNode& ExpressionStatementASTNode::GetExpression() const
{
	return *m_expression;
}

void ExpressionStatementASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

AssertStatementASTNode::AssertStatementASTNode(ASTNodePtr condition, const int sourceLine, std::string sourceText)
	: m_condition(std::move(condition))
	, m_sourceLine(sourceLine)
	, m_sourceText(std::move(sourceText))
{
}

const ASTNode& AssertStatementASTNode::GetCondition() const
{
	return *m_condition;
}

int AssertStatementASTNode::GetSourceLine() const
{
	return m_sourceLine;
}

const std::string& AssertStatementASTNode::GetSourceText() const
{
	return m_sourceText;
}

void AssertStatementASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

void EmptyStatementASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

ProgramASTNode::ProgramASTNode(ASTNodePtr statements)
	: m_statements(std::move(statements))
{
}

const ASTNode& ProgramASTNode::GetStatements() const
{
	return *m_statements;
}

void ProgramASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

StatementListASTNode::StatementListASTNode(std::vector<ASTNodePtr> statements)
	: m_statements(std::move(statements))
{
}

const std::vector<ASTNodePtr>& StatementListASTNode::GetStatements() const
{
	return m_statements;
}

std::vector<ASTNodePtr> StatementListASTNode::TakeStatements()
{
	return std::move(m_statements);
}

void StatementListASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

BlockASTNode::BlockASTNode(ASTNodePtr statements)
	: m_statements(std::move(statements))
{
}

const ASTNode& BlockASTNode::GetStatements() const
{
	return *m_statements;
}

void BlockASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

IfASTNode::IfASTNode(ASTNodePtr condition, ASTNodePtr thenBranch, ASTNodePtr elseBranch)
	: m_condition(std::move(condition))
	, m_thenBranch(std::move(thenBranch))
	, m_elseBranch(std::move(elseBranch))
{
}

const ASTNode& IfASTNode::GetCondition() const
{
	return *m_condition;
}

const ASTNode& IfASTNode::GetThenBranch() const
{
	return *m_thenBranch;
}

const ASTNode* IfASTNode::GetElseBranch() const
{
	return m_elseBranch.get();
}

void IfASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

ForASTNode::ForASTNode(ASTNodePtr initializer, ASTNodePtr condition, ASTNodePtr post, ASTNodePtr body)
	: m_initializer(std::move(initializer))
	, m_condition(std::move(condition))
	, m_post(std::move(post))
	, m_body(std::move(body))
{
}

const ASTNode* ForASTNode::GetInitializer() const
{
	return m_initializer.get();
}

const ASTNode& ForASTNode::GetCondition() const
{
	return *m_condition;
}

const ASTNode* ForASTNode::GetPost() const
{
	return m_post.get();
}

const ASTNode& ForASTNode::GetBody() const
{
	return *m_body;
}

void ForASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

void BreakASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

void ContinueASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

ReturnASTNode::ReturnASTNode() = default;

ReturnASTNode::ReturnASTNode(std::vector<ASTNodePtr> values)
	: m_values(std::move(values))
{
}

bool ReturnASTNode::HasValues() const
{
	return !m_values.empty();
}

const std::vector<ASTNodePtr>& ReturnASTNode::GetValues() const
{
	return m_values;
}

void ReturnASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

FunctionDeclarationASTNode::FunctionDeclarationASTNode(
	std::string name,
	std::vector<FunctionParameter> parameters,
	const std::optional<TypeDescriptor>& returnType,
	ASTNodePtr body)
	: m_receiver(std::nullopt)
	, m_name(std::move(name))
	, m_parameters(std::move(parameters))
	, m_returnType(returnType)
	, m_body(std::move(body))
{
}

FunctionDeclarationASTNode::FunctionDeclarationASTNode(
	FunctionParameter receiver,
	std::string name,
	std::vector<FunctionParameter> parameters,
	const std::optional<TypeDescriptor>& returnType,
	ASTNodePtr body)
	: m_receiver(std::move(receiver))
	, m_name(std::move(name))
	, m_parameters(std::move(parameters))
	, m_returnType(returnType)
	, m_body(std::move(body))
{
}

const std::string& FunctionDeclarationASTNode::GetName() const
{
	return m_name;
}

std::string FunctionDeclarationASTNode::GetQualifiedName() const
{
	if (!m_receiver.has_value())
	{
		return m_name;
	}

	return m_receiver->type.ToString() + "." + m_name;
}

bool FunctionDeclarationASTNode::IsMethod() const
{
	return m_receiver.has_value();
}

const FunctionParameter* FunctionDeclarationASTNode::GetReceiver() const
{
	return m_receiver ? &*m_receiver : nullptr;
}

const std::vector<FunctionParameter>& FunctionDeclarationASTNode::GetParameters() const
{
	return m_parameters;
}

TypeDescriptor FunctionDeclarationASTNode::GetReturnType() const
{
	return m_returnType.value_or(Type::VOID);
}

const ASTNode& FunctionDeclarationASTNode::GetBody() const
{
	return *m_body;
}

void FunctionDeclarationASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

StructDeclarationASTNode::StructDeclarationASTNode(std::string name, std::vector<StructField> fields)
	: m_name(std::move(name))
	, m_fields(std::move(fields))
{
}

const std::string& StructDeclarationASTNode::GetName() const
{
	return m_name;
}

const std::vector<StructField>& StructDeclarationASTNode::GetFields() const
{
	return m_fields;
}

void StructDeclarationASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}

TestDeclarationASTNode::TestDeclarationASTNode(std::string name, ASTNodePtr body)
	: m_name(std::move(name))
	, m_body(std::move(body))
{
}

const std::string& TestDeclarationASTNode::GetName() const
{
	return m_name;
}

const ASTNode& TestDeclarationASTNode::GetBody() const
{
	return *m_body;
}

void TestDeclarationASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}
