#pragma once

class BoolLiteralASTNode;
class IntLiteralASTNode;
class FloatLiteralASTNode;
class IdentifierASTNode;
class UnaryASTNode;
class BinaryASTNode;
class MemberAccessASTNode;
class IndexASTNode;
class AssignmentASTNode;
class SequenceASTNode;
class IfElseASTNode;
class PrintfASTNode;

class ASTNodeVisitor
{
public:
	virtual ~ASTNodeVisitor() = default;

	virtual void Visit(const BoolLiteralASTNode& expr) = 0;
	virtual void Visit(const IntLiteralASTNode& expr) = 0;
	virtual void Visit(const FloatLiteralASTNode& expr) = 0;
	virtual void Visit(const IdentifierASTNode& expr) = 0;
	virtual void Visit(const UnaryASTNode& expr) = 0;
	virtual void Visit(const BinaryASTNode& expr) = 0;
	virtual void Visit(const MemberAccessASTNode& expr) = 0;
	virtual void Visit(const IndexASTNode& expr) = 0;
	virtual void Visit(const AssignmentASTNode& expr) = 0;
	virtual void Visit(const SequenceASTNode& expr) = 0;
	virtual void Visit(const IfElseASTNode& expr) = 0;
	virtual void Visit(const PrintfASTNode& expr) = 0;
};
