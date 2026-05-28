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
class ProgramASTNode;
class StatementListASTNode;
class BlockASTNode;
class IfASTNode;
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
	virtual void Visit(const ProgramASTNode& expr) = 0;
	virtual void Visit(const StatementListASTNode& expr) = 0;
	virtual void Visit(const BlockASTNode& expr) = 0;
	virtual void Visit(const IfASTNode& expr) = 0;
	virtual void Visit(const PrintfASTNode& expr) = 0;
};
