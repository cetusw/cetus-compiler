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

	[[nodiscard]] std::optional<Type> GetInferredType() const;
	void SetInferredType(Type type) const;

private:
	mutable std::optional<Type> m_inferredType;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

class BoolLiteralASTNode final : public ASTNode
{
public:
	explicit BoolLiteralASTNode(bool value);

	[[nodiscard]] bool GetValue() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	bool m_value;
};

class IntLiteralASTNode final : public ASTNode
{
public:
	explicit IntLiteralASTNode(std::string value);

	[[nodiscard]] const std::string& GetValue() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::string m_value;
};

class FloatLiteralASTNode final : public ASTNode
{
public:
	explicit FloatLiteralASTNode(std::string value);

	[[nodiscard]] const std::string& GetValue() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::string m_value;
};

class IdentifierASTNode final : public ASTNode
{
public:
	explicit IdentifierASTNode(std::string name);

	[[nodiscard]] const std::string& GetName() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::string m_name;
};

class UnaryASTNode final : public ASTNode
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

class BinaryASTNode final : public ASTNode
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

class MemberAccessASTNode final : public ASTNode
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

class IndexASTNode final : public ASTNode
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

class AssignmentASTNode final : public ASTNode
{
public:
	AssignmentASTNode(std::string name, ASTNodePtr value);

	[[nodiscard]] const std::string& GetName() const;
	[[nodiscard]] const ASTNode& GetValue() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::string m_name;
	ASTNodePtr m_value;
};

class SequenceASTNode final : public ASTNode
{
public:
	explicit SequenceASTNode(std::vector<ASTNodePtr> expressions);

	[[nodiscard]] const std::vector<ASTNodePtr>& GetExpressions() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	std::vector<ASTNodePtr> m_expressions;
};

class IfElseASTNode final : public ASTNode
{
public:
	IfElseASTNode(ASTNodePtr condition, ASTNodePtr thenBranch, ASTNodePtr elseBranch);

	[[nodiscard]] const ASTNode& GetCondition() const;
	[[nodiscard]] const ASTNode& GetThenBranch() const;
	[[nodiscard]] const ASTNode& GetElseBranch() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_condition;
	ASTNodePtr m_thenBranch;
	ASTNodePtr m_elseBranch;
};

class PrintfASTNode final : public ASTNode
{
public:
	explicit PrintfASTNode(ASTNodePtr argument);

	[[nodiscard]] const ASTNode& GetArgument() const;
	void Accept(ASTNodeVisitor& visitor) const override;

private:
	ASTNodePtr m_argument;
};
