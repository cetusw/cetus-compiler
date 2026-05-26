#include "SemanticAnalyzer.h"

#include "rules/TypeRules.h"
#include "src/frontend/syntax/ast/ASTNode.h"

SemanticAnalyzer::SemanticAnalyzer(SymbolTable symbols)
	: m_symbolTable(std::move(symbols))
{
}

TypeCheckResult SemanticAnalyzer::Analyze(const ASTNode& node)
{
	m_currentType = Type::ERROR;
	m_diagnostics.clear();
	node.Accept(*this);
	if (!m_diagnostics.empty())
	{
		return TypeCheckResult::Error(std::move(m_diagnostics), std::move(m_symbolTable));
	}

	return TypeCheckResult::Success(m_currentType, std::move(m_symbolTable));
}

void SemanticAnalyzer::Visit(const BoolLiteralASTNode& node)
{
	SetCurrentType(node, Type::BOOL);
}

void SemanticAnalyzer::Visit(const IntLiteralASTNode& node)
{
	SetCurrentType(node, Type::INT);
}

void SemanticAnalyzer::Visit(const FloatLiteralASTNode& node)
{
	SetCurrentType(node, Type::FLOAT);
}

void SemanticAnalyzer::Visit(const IdentifierASTNode& node)
{
	const SemanticSymbol* symbol = m_symbolTable.Resolve(node.GetName());
	if (!symbol)
	{
		AddDiagnostic("Undefined identifier: " + node.GetName());
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetCurrentType(node, symbol->type);
}

void SemanticAnalyzer::Visit(const UnaryASTNode& node)
{
	const Type operandType = AnalyzeChild(node.GetOperand());
	if (operandType == Type::ERROR)
	{
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetTypeCheckResult(node, TypeRules::CheckUnaryOperator(node.GetOperator(), operandType));
}

void SemanticAnalyzer::Visit(const BinaryASTNode& node)
{
	const Type leftType = AnalyzeChild(node.GetLeft());
	const Type rightType = AnalyzeChild(node.GetRight());
	if (leftType == Type::ERROR || rightType == Type::ERROR)
	{
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetTypeCheckResult(node, TypeRules::CheckBinaryOperator(node.GetOperator(), leftType, rightType));
}

void SemanticAnalyzer::Visit(const MemberAccessASTNode& node)
{
	AddDiagnostic("Member access type checking is not implemented yet.");
	SetCurrentType(node, Type::ERROR);
}

void SemanticAnalyzer::Visit(const IndexASTNode& node)
{
	AddDiagnostic("Index access type checking is not implemented yet.");
	SetCurrentType(node, Type::ERROR);
}

void SemanticAnalyzer::Visit(const AssignmentASTNode& node)
{
	const Type valueType = AnalyzeChild(node.GetValue());
	if (valueType == Type::ERROR)
	{
		SetCurrentType(node, Type::ERROR);
		return;
	}

	const SemanticSymbol* existing = m_symbolTable.Resolve(node.GetName());
	if (existing && existing->type != valueType)
	{
		AddDiagnostic("Cannot assign value of different type to identifier: " + node.GetName());
		SetCurrentType(node, Type::ERROR);
		return;
	}
	if (!existing)
	{
		m_symbolTable.Define(SemanticSymbol{ node.GetName(), valueType });
	}

	SetCurrentType(node, valueType);
}

void SemanticAnalyzer::Visit(const SequenceASTNode& node)
{
	if (node.GetExpressions().empty())
	{
		AddDiagnostic("Sequence expression cannot be empty.");
		SetCurrentType(node, Type::ERROR);
		return;
	}

	Type lastType = Type::ERROR;
	bool hasChildError = false;
	for (const ASTNodePtr& child : node.GetExpressions())
	{
		lastType = AnalyzeChild(*child);
		if (lastType == Type::ERROR)
		{
			hasChildError = true;
		}
	}

	SetCurrentType(node, hasChildError ? Type::ERROR : lastType);
}

void SemanticAnalyzer::Visit(const IfASTNode& node)
{
	const Type conditionType = AnalyzeChild(node.GetCondition());
	const Type thenType = AnalyzeChild(node.GetThenBranch());
	const ASTNode* elseBranch = node.GetElseBranch();
	const Type elseType = elseBranch ? AnalyzeChild(*elseBranch) : thenType;

	bool hasError = false;
	if (!IsFalsey(conditionType))
	{
		AddDiagnostic("If condition expects truthy-compatible expression.");
		hasError = true;
	}
	if (thenType == Type::ERROR || elseType == Type::ERROR)
	{
		hasError = true;
	}
	if (!hasError && elseBranch && thenType != elseType)
	{
		AddDiagnostic("If branches must have the same type.");
		hasError = true;
	}

	SetCurrentType(node, hasError ? Type::ERROR : thenType);
}

void SemanticAnalyzer::Visit(const PrintfASTNode& node)
{
	const Type argumentType = AnalyzeChild(node.GetArgument());
	if (!IsFalsey(argumentType))
	{
		AddDiagnostic("printf expects int or bool argument.");
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetCurrentType(node, argumentType);
}

Type SemanticAnalyzer::AnalyzeChild(const ASTNode& node)
{
	node.Accept(*this);
	return m_currentType;
}

// TODO избавиться от дублирования кода
bool SemanticAnalyzer::IsFalsey(const Type type)
{
	return type == Type::INT || type == Type::BOOL;
}

void SemanticAnalyzer::SetCurrentType(const ASTNode& node, const Type type)
{
	m_currentType = type;
	node.SetInferredType(type);
}

void SemanticAnalyzer::SetTypeCheckResult(const ASTNode& node, TypeCheckResult result)
{
	if (!result.diagnostics.empty())
	{
		AddDiagnostics(std::move(result.diagnostics));
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetCurrentType(node, result.type);
}

void SemanticAnalyzer::AddDiagnostic(std::string message)
{
	m_diagnostics.push_back({ std::move(message), 0 });
}

void SemanticAnalyzer::AddDiagnostics(std::vector<SemanticDiagnostic> diagnostics)
{
	for (SemanticDiagnostic& diagnostic : diagnostics)
	{
		m_diagnostics.push_back(std::move(diagnostic));
	}
}
