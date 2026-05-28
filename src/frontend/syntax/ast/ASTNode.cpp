#include "ASTNode.h"

#include <utility>

std::optional<Type> ASTNode::GetInferredType() const
{
	return m_inferredType;
}

void ASTNode::SetInferredType(const Type type) const
{
	m_inferredType = type;
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

AssignmentASTNode::AssignmentASTNode(std::vector<std::string> names, std::vector<ASTNodePtr> values)
	: m_names(std::move(names))
	, m_values(std::move(values))
{
}

const std::vector<std::string>& AssignmentASTNode::GetNames() const
{
	return m_names;
}

const std::vector<ASTNodePtr>& AssignmentASTNode::GetValues() const
{
	return m_values;
}

void AssignmentASTNode::Accept(ASTNodeVisitor& visitor) const
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
	std::optional<Type> declaredType,
	std::vector<ASTNodePtr> values)
	: m_names(std::move(names))
	, m_declaredType(declaredType)
	, m_values(std::move(values))
{
}

const std::vector<std::string>& VariableDeclarationASTNode::GetNames() const
{
	return m_names;
}

std::optional<Type> VariableDeclarationASTNode::GetDeclaredType() const
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

PrintfASTNode::PrintfASTNode(ASTNodePtr argument)
	: m_argument(std::move(argument))
{
}

const ASTNode& PrintfASTNode::GetArgument() const
{
	return *m_argument;
}

void PrintfASTNode::Accept(ASTNodeVisitor& visitor) const
{
	visitor.Visit(*this);
}
