#pragma once

class BoolLiteralExpr;
class IntLiteralExpr;
class FloatLiteralExpr;
class IdentifierExpr;
class UnaryExpr;
class BinaryExpr;
class MemberAccessExpr;
class IndexExpr;

class ExprVisitor
{
public:
	virtual ~ExprVisitor() = default;

	virtual void Visit(const BoolLiteralExpr& expr) = 0;
	virtual void Visit(const IntLiteralExpr& expr) = 0;
	virtual void Visit(const FloatLiteralExpr& expr) = 0;
	virtual void Visit(const IdentifierExpr& expr) = 0;
	virtual void Visit(const UnaryExpr& expr) = 0;
	virtual void Visit(const BinaryExpr& expr) = 0;
	virtual void Visit(const MemberAccessExpr& expr) = 0;
	virtual void Visit(const IndexExpr& expr) = 0;
};
