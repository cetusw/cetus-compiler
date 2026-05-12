#pragma once

#include "TypeCheckResult.h"
#include "SymbolTable.h"
#include "src/frontend/syntax/ast/Expr.h"
#include <string>

class TypeCheckerVisitor final : public ExprVisitor
{
public:
	explicit TypeCheckerVisitor(SymbolTable symbols);

	[[nodiscard]] TypeCheckResult Check(const Expr& expr);

	void Visit(const BoolLiteralExpr& expr) override;
	void Visit(const IntLiteralExpr& expr) override;
	void Visit(const FloatLiteralExpr& expr) override;
	void Visit(const IdentifierExpr& expr) override;
	void Visit(const UnaryExpr& expr) override;
	void Visit(const BinaryExpr& expr) override;
	void Visit(const MemberAccessExpr& expr) override;
	void Visit(const IndexExpr& expr) override;
	void Visit(const AssignmentExpr& expr) override;
	void Visit(const SequenceExpr& expr) override;

private:
	[[nodiscard]] Type CheckChild(const Expr& expr);
	void SetCurrentType(const Expr& expr, Type type);
	void SetTypeCheckResult(const Expr& expr, TypeCheckResult result);
	void Fail(std::string message);

	SymbolTable m_symbols;
	Type m_currentType = Type::ERROR;
	std::optional<std::string> m_error;
	TypeCheckResult::NodeTypes m_nodeTypes;
};
