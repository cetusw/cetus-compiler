#include "ASTNode.h"

#include <utility>

std::optional<TypeDescriptor> ASTNode::GetInferredType() const
{
	return m_inferredType;
}

void ASTNode::SetInferredType(TypeDescriptor type) const
{
	m_inferredType = std::move(type);
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

ReturnASTNode::ReturnASTNode(ASTNodePtr value)
	: m_value(std::move(value))
{
}

const ASTNode* ReturnASTNode::GetValue() const
{
	return m_value.get();
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
