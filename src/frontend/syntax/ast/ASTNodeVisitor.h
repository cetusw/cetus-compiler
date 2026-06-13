#pragma once

class BoolLiteralASTNode;
class NilLiteralASTNode;
class IntLiteralASTNode;
class FloatLiteralASTNode;
class StringLiteralASTNode;
class ArrayLiteralASTNode;
class StructLiteralASTNode;
class IdentifierASTNode;
class AddressOfASTNode;
class UnaryASTNode;
class BinaryASTNode;
class MemberAccessASTNode;
class IndexASTNode;
class SliceExpressionASTNode;
class CallExpressionASTNode;
class AssignmentASTNode;
class IncrementASTNode;
class DecrementASTNode;
class ShortVariableDeclarationASTNode;
class VariableDeclarationASTNode;
class ExpressionStatementASTNode;
class EmptyStatementASTNode;
class ProgramASTNode;
class StatementListASTNode;
class BlockASTNode;
class IfASTNode;
class ForASTNode;
class BreakASTNode;
class ContinueASTNode;
class ReturnASTNode;
class FunctionDeclarationASTNode;
class StructDeclarationASTNode;

class ASTNodeVisitor
{
public:
	virtual ~ASTNodeVisitor() = default;

	virtual void Visit(const BoolLiteralASTNode& expr) = 0;
	virtual void Visit(const NilLiteralASTNode& expr) = 0;
	virtual void Visit(const IntLiteralASTNode& expr) = 0;
	virtual void Visit(const FloatLiteralASTNode& expr) = 0;
	virtual void Visit(const StringLiteralASTNode& expr) = 0;
	virtual void Visit(const ArrayLiteralASTNode& expr) = 0;
	virtual void Visit(const StructLiteralASTNode& expr) = 0;
	virtual void Visit(const IdentifierASTNode& expr) = 0;
	virtual void Visit(const AddressOfASTNode& expr) = 0;
	virtual void Visit(const UnaryASTNode& expr) = 0;
	virtual void Visit(const BinaryASTNode& expr) = 0;
	virtual void Visit(const MemberAccessASTNode& expr) = 0;
	virtual void Visit(const IndexASTNode& expr) = 0;
	virtual void Visit(const SliceExpressionASTNode& expr) = 0;
	virtual void Visit(const CallExpressionASTNode& expr) = 0;
	virtual void Visit(const AssignmentASTNode& expr) = 0;
	virtual void Visit(const IncrementASTNode& expr) = 0;
	virtual void Visit(const DecrementASTNode& expr) = 0;
	virtual void Visit(const ShortVariableDeclarationASTNode& expr) = 0;
	virtual void Visit(const VariableDeclarationASTNode& expr) = 0;
	virtual void Visit(const ExpressionStatementASTNode& expr) = 0;
	virtual void Visit(const EmptyStatementASTNode& expr) = 0;
	virtual void Visit(const ProgramASTNode& expr) = 0;
	virtual void Visit(const StatementListASTNode& expr) = 0;
	virtual void Visit(const BlockASTNode& expr) = 0;
	virtual void Visit(const IfASTNode& expr) = 0;
	virtual void Visit(const ForASTNode& expr) = 0;
	virtual void Visit(const BreakASTNode& expr) = 0;
	virtual void Visit(const ContinueASTNode& expr) = 0;
	virtual void Visit(const ReturnASTNode& expr) = 0;
	virtual void Visit(const FunctionDeclarationASTNode& expr) = 0;
	virtual void Visit(const StructDeclarationASTNode& expr) = 0;
};
