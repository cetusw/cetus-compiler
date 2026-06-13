#pragma once

#include "ASTNodeVisitor.h"
#include "src/frontend/semantic/Type.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

enum class UnaryOperator
{
	NEGATE,
	NOT
};

enum class BinaryOperator
{
	OR,
	AND,
	ADD,
	SUBTRACT,
	MULTIPLY,
	DIVIDE,
	MODULO,
	LESS,
	LESS_EQUAL,
	NOT_EQUAL,
	EQUAL,
	GREATER,
	GREATER_EQUAL
};

class ASTNode
{
public:
	virtual ~ASTNode() = default;
	virtual void Accept(ASTNodeVisitor& visitor) const = 0;

	[[nodiscard]] std::optional<TypeDescriptor> GetInferredType() const;
	void SetInferredType(TypeDescriptor type) const;

private:
	mutable std::optional<TypeDescriptor> m_inferredType;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

[[nodiscard]] std::string DescribeExpression(const ASTNode& node);

struct FunctionParameter
{
	std::string name;
	TypeDescriptor type = Type::ERROR;
	bool isPointer = false;
};

struct StructField
{
	std::string name;
	TypeDescriptor type = Type::ERROR;
};

struct StructFieldInitializer
{
	std::string name;
	ASTNodePtr expression;
};

class StatementASTNode : public ASTNode
{
};

class ExpressionASTNode : public ASTNode
{
};

class BoolLiteralASTNode final : public ExpressionASTNode
{
public:
	explicit BoolLiteralASTNode(bool value);

	[[nodiscard]] bool GetValue() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	bool m_value;
};

class NilLiteralASTNode final : public ExpressionASTNode
{
public:
	void Accept(ASTNodeVisitor& visitor) const override;
};

class IntLiteralASTNode final : public ExpressionASTNode
{
public:
	explicit IntLiteralASTNode(std::string value);

	[[nodiscard]] const std::string& GetValue() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::string m_value;
};

class FloatLiteralASTNode final : public ExpressionASTNode
{
public:
	explicit FloatLiteralASTNode(std::string value);

	[[nodiscard]] const std::string& GetValue() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::string m_value;
};

class StringLiteralASTNode final : public ExpressionASTNode
{
public:
	explicit StringLiteralASTNode(std::string value);

	[[nodiscard]] const std::string& GetValue() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::string m_value;
};

class ArrayLiteralASTNode final : public ExpressionASTNode
{
public:
	ArrayLiteralASTNode(TypeDescriptor type, std::vector<ASTNodePtr> elements);

	[[nodiscard]] const TypeDescriptor& GetType() const;
	[[nodiscard]] const std::vector<ASTNodePtr>& GetElements() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	TypeDescriptor m_type;
	std::vector<ASTNodePtr> m_elements;
};

class StructLiteralASTNode final : public ExpressionASTNode
{
public:
	StructLiteralASTNode(std::string typeName, std::vector<StructFieldInitializer> initializers);

	[[nodiscard]] const std::string& GetTypeName() const;
	[[nodiscard]] const std::vector<StructFieldInitializer>& GetInitializers() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::string m_typeName;
	std::vector<StructFieldInitializer> m_initializers;
};

class IdentifierASTNode final : public ExpressionASTNode
{
public:
	explicit IdentifierASTNode(std::string name);

	[[nodiscard]] const std::string& GetName() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::string m_name;
};

class AddressOfASTNode final : public ExpressionASTNode
{
public:
	explicit AddressOfASTNode(ASTNodePtr target);

	[[nodiscard]] const ASTNode& GetTarget() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_target;
};

class UnaryASTNode final : public ExpressionASTNode
{
public:
	UnaryASTNode(UnaryOperator op, ASTNodePtr operand);

	[[nodiscard]] UnaryOperator GetOperator() const;
	[[nodiscard]] const ASTNode& GetOperand() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	UnaryOperator m_op;
	ASTNodePtr m_operand;
};

class BinaryASTNode final : public ExpressionASTNode
{
public:
	BinaryASTNode(ASTNodePtr left, BinaryOperator op, ASTNodePtr right);

	[[nodiscard]] const ASTNode& GetLeft() const;
	[[nodiscard]] BinaryOperator GetOperator() const;
	[[nodiscard]] const ASTNode& GetRight() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_left;
	BinaryOperator m_op;
	ASTNodePtr m_right;
};

class MemberAccessASTNode final : public ExpressionASTNode
{
public:
	MemberAccessASTNode(ASTNodePtr object, std::string member);

	[[nodiscard]] const ASTNode& GetObject() const;
	[[nodiscard]] const std::string& GetMember() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_object;
	std::string m_member;
};

class IndexASTNode final : public ExpressionASTNode
{
public:
	IndexASTNode(ASTNodePtr object, ASTNodePtr index);

	[[nodiscard]] const ASTNode& GetObject() const;
	[[nodiscard]] const ASTNode& GetIndex() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_object;
	ASTNodePtr m_index;
};

class SliceExpressionASTNode final : public ExpressionASTNode
{
public:
	SliceExpressionASTNode(ASTNodePtr object, ASTNodePtr start, ASTNodePtr end);

	[[nodiscard]] const ASTNode& GetObject() const;
	[[nodiscard]] const ASTNode* GetStart() const;
	[[nodiscard]] const ASTNode* GetEnd() const;
	[[nodiscard]] bool HasStart() const;
	[[nodiscard]] bool HasEnd() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_object;
	ASTNodePtr m_start;
	ASTNodePtr m_end;
};

class CallExpressionASTNode final : public ExpressionASTNode
{
public:
	CallExpressionASTNode(std::string calleeName, std::vector<ASTNodePtr> arguments);
	CallExpressionASTNode(ASTNodePtr receiver, std::string calleeName, std::vector<ASTNodePtr> arguments);

	[[nodiscard]] const std::string& GetCalleeName() const;
	[[nodiscard]] bool IsMethodCall() const;
	[[nodiscard]] const ASTNode* GetReceiver() const;
	[[nodiscard]] const std::vector<ASTNodePtr>& GetArguments() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_receiver;
	std::string m_calleeName;
	std::vector<ASTNodePtr> m_arguments;
};

class AssignmentASTNode final : public StatementASTNode
{
public:
	AssignmentASTNode(std::vector<ASTNodePtr> targets, std::vector<ASTNodePtr> values);

	[[nodiscard]] const std::vector<ASTNodePtr>& GetTargets() const;
	[[nodiscard]] const std::vector<ASTNodePtr>& GetValues() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::vector<ASTNodePtr> m_targets;
	std::vector<ASTNodePtr> m_values;
};

class IncrementASTNode final : public StatementASTNode
{
public:
	explicit IncrementASTNode(ASTNodePtr target);

	[[nodiscard]] const ASTNode& GetTarget() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_target;
};

class DecrementASTNode final : public StatementASTNode
{
public:
	explicit DecrementASTNode(ASTNodePtr target);

	[[nodiscard]] const ASTNode& GetTarget() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_target;
};

class ShortVariableDeclarationASTNode final : public StatementASTNode
{
public:
	ShortVariableDeclarationASTNode(std::vector<std::string> names, std::vector<ASTNodePtr> values);

	[[nodiscard]] const std::vector<std::string>& GetNames() const;
	[[nodiscard]] const std::vector<ASTNodePtr>& GetValues() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::vector<std::string> m_names;
	std::vector<ASTNodePtr> m_values;
};

class VariableDeclarationASTNode final : public StatementASTNode
{
public:
	VariableDeclarationASTNode(
		std::vector<std::string> names,
		std::optional<TypeDescriptor> declaredType,
		std::vector<ASTNodePtr> values);

	[[nodiscard]] const std::vector<std::string>& GetNames() const;
	[[nodiscard]] std::optional<TypeDescriptor> GetDeclaredType() const;
	[[nodiscard]] const std::vector<ASTNodePtr>& GetValues() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::vector<std::string> m_names;
	std::optional<TypeDescriptor> m_declaredType;
	std::vector<ASTNodePtr> m_values;
};

class ExpressionStatementASTNode final : public StatementASTNode
{
public:
	explicit ExpressionStatementASTNode(ASTNodePtr expression);

	[[nodiscard]] const ASTNode& GetExpression() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_expression;
};

class AssertStatementASTNode final : public StatementASTNode
{
public:
	AssertStatementASTNode(ASTNodePtr condition, int sourceLine, std::string sourceText);

	[[nodiscard]] const ASTNode& GetCondition() const;
	[[nodiscard]] int GetSourceLine() const;
	[[nodiscard]] const std::string& GetSourceText() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_condition;
	int m_sourceLine;
	std::string m_sourceText;
};

class EmptyStatementASTNode final : public StatementASTNode
{
public:
	void Accept(ASTNodeVisitor& visitor) const override;
};

class ProgramASTNode final : public StatementASTNode
{
public:
	explicit ProgramASTNode(ASTNodePtr statements);

	[[nodiscard]] const ASTNode& GetStatements() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_statements;
};

class StatementListASTNode final : public StatementASTNode
{
public:
	explicit StatementListASTNode(std::vector<ASTNodePtr> statements);

	[[nodiscard]] const std::vector<ASTNodePtr>& GetStatements() const;
	[[nodiscard]] std::vector<ASTNodePtr> TakeStatements();
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::vector<ASTNodePtr> m_statements;
};

class BlockASTNode final : public StatementASTNode
{
public:
	explicit BlockASTNode(ASTNodePtr statements);

	[[nodiscard]] const ASTNode& GetStatements() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_statements;
};

class IfASTNode final : public StatementASTNode
{
public:
	IfASTNode(ASTNodePtr condition, ASTNodePtr thenBranch, ASTNodePtr elseBranch = nullptr);

	[[nodiscard]] const ASTNode& GetCondition() const;
	[[nodiscard]] const ASTNode& GetThenBranch() const;
	[[nodiscard]] const ASTNode* GetElseBranch() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_condition;
	ASTNodePtr m_thenBranch;
	ASTNodePtr m_elseBranch;
};

class ForASTNode final : public StatementASTNode
{
public:
	ForASTNode(ASTNodePtr initializer, ASTNodePtr condition, ASTNodePtr post, ASTNodePtr body);

	[[nodiscard]] const ASTNode* GetInitializer() const;
	[[nodiscard]] const ASTNode& GetCondition() const;
	[[nodiscard]] const ASTNode* GetPost() const;
	[[nodiscard]] const ASTNode& GetBody() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_initializer;
	ASTNodePtr m_condition;
	ASTNodePtr m_post;
	ASTNodePtr m_body;
};

class BreakASTNode final : public StatementASTNode
{
public:
	void Accept(ASTNodeVisitor& visitor) const override;
};

class ContinueASTNode final : public StatementASTNode
{
public:
	void Accept(ASTNodeVisitor& visitor) const override;
};

class ReturnASTNode final : public StatementASTNode
{
public:
	ReturnASTNode();
	explicit ReturnASTNode(std::vector<ASTNodePtr> values);

	[[nodiscard]] bool HasValues() const;
	[[nodiscard]] const std::vector<ASTNodePtr>& GetValues() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::vector<ASTNodePtr> m_values;
};

class FunctionDeclarationASTNode final : public StatementASTNode
{
public:
	FunctionDeclarationASTNode(
		std::string name,
		std::vector<FunctionParameter> parameters,
		const std::optional<TypeDescriptor>& returnType,
		ASTNodePtr body);
	FunctionDeclarationASTNode(
		FunctionParameter receiver,
		std::string name,
		std::vector<FunctionParameter> parameters,
		const std::optional<TypeDescriptor>& returnType,
		ASTNodePtr body);

	[[nodiscard]] const std::string& GetName() const;
	[[nodiscard]] std::string GetQualifiedName() const;
	[[nodiscard]] bool IsMethod() const;
	[[nodiscard]] const FunctionParameter* GetReceiver() const;
	[[nodiscard]] const std::vector<FunctionParameter>& GetParameters() const;
	[[nodiscard]] TypeDescriptor GetReturnType() const;
	[[nodiscard]] const ASTNode& GetBody() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::optional<FunctionParameter> m_receiver;
	std::string m_name;
	std::vector<FunctionParameter> m_parameters;
	std::optional<TypeDescriptor> m_returnType;
	ASTNodePtr m_body;
};

class StructDeclarationASTNode final : public StatementASTNode
{
public:
	StructDeclarationASTNode(std::string name, std::vector<StructField> fields);

	[[nodiscard]] const std::string& GetName() const;
	[[nodiscard]] const std::vector<StructField>& GetFields() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::string m_name;
	std::vector<StructField> m_fields;
};

class TestDeclarationASTNode final : public StatementASTNode
{
public:
	TestDeclarationASTNode(std::string name, ASTNodePtr body);

	[[nodiscard]] const std::string& GetName() const;
	[[nodiscard]] const ASTNode& GetBody() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::string m_name;
	ASTNodePtr m_body;
};
