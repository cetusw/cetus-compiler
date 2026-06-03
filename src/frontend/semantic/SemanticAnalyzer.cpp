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
	m_predeclaredFunctions.clear();
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

void SemanticAnalyzer::Visit(const StringLiteralASTNode& node)
{
	SetCurrentType(node, Type::STRING);
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

	if (symbol->kind == SemanticSymbolKind::FUNCTION)
	{
		AddDiagnostic("Function identifier cannot be used as value: " + node.GetName());
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

void SemanticAnalyzer::Visit(const CallExpressionASTNode& node)
{
	const SemanticSymbol* symbol = m_symbolTable.Resolve(node.GetCalleeName());
	if (!symbol)
	{
		AddDiagnostic("Undefined function: " + node.GetCalleeName());
		SetCurrentType(node, Type::ERROR);
		return;
	}
	if (symbol->kind != SemanticSymbolKind::FUNCTION)
	{
		AddDiagnostic("Identifier is not a function: " + node.GetCalleeName());
		SetCurrentType(node, Type::ERROR);
		return;
	}

	const std::vector<Type> argumentTypes = AnalyzeValues(node.GetArguments());
	bool hasError = HasError(argumentTypes);
	if (symbol->parameterTypes.size() != argumentTypes.size())
	{
		AddDiagnostic("Function call argument count does not match function parameters: " + node.GetCalleeName());
		SetCurrentType(node, Type::ERROR);
		return;
	}

	for (std::size_t index = 0; index < argumentTypes.size(); ++index)
	{
		if (argumentTypes[index] == Type::ERROR)
		{
			continue;
		}
		if (argumentTypes[index] != symbol->parameterTypes[index])
		{
			AddDiagnostic("Function call argument type does not match parameter type: " + node.GetCalleeName());
			hasError = true;
		}
	}

	SetCurrentType(node, hasError ? Type::ERROR : symbol->type);
}

void SemanticAnalyzer::Visit(const AssignmentASTNode& node)
{
	const std::size_t diagnosticCount = m_diagnostics.size();
	const std::vector<Type> valueTypes = AnalyzeValues(node.GetValues());
	ValidateAssignment(node.GetNames(), valueTypes);
	SetCurrentType(node, HasError(valueTypes) || m_diagnostics.size() != diagnosticCount ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const ShortVariableDeclarationASTNode& node)
{
	const std::size_t diagnosticCount = m_diagnostics.size();
	const std::vector<Type> valueTypes = AnalyzeValues(node.GetValues());
	DefineShortVariables(node.GetNames(), valueTypes);
	SetCurrentType(node, HasError(valueTypes) || m_diagnostics.size() != diagnosticCount ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const VariableDeclarationASTNode& node)
{
	const std::size_t diagnosticCount = m_diagnostics.size();
	const std::vector<Type> valueTypes = AnalyzeValues(node.GetValues());
	DefineVariables(node.GetNames(), node.GetDeclaredType(), valueTypes);
	SetCurrentType(node, HasError(valueTypes) || m_diagnostics.size() != diagnosticCount ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const ExpressionStatementASTNode& node)
{
	const Type expressionType = AnalyzeChild(node.GetExpression());
	SetCurrentType(node, expressionType == Type::ERROR ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::ValidateAssignment(const std::vector<std::string>& names, const std::vector<Type>& valueTypes)
{
	if (names.size() != valueTypes.size())
	{
		AddDiagnostic("Assignment expects the same number of targets and values.");
		return;
	}

	for (std::size_t index = 0; index < names.size(); ++index)
	{
		if (valueTypes[index] == Type::ERROR)
		{
			continue;
		}

		const SemanticSymbol* existing = m_symbolTable.Resolve(names[index]);
		if (!existing)
		{
			AddDiagnostic("Cannot assign to undefined identifier: " + names[index]);
			continue;
		}
		if (existing->kind != SemanticSymbolKind::VARIABLE)
		{
			AddDiagnostic("Cannot assign to non-variable identifier: " + names[index]);
			continue;
		}
		if (existing->type != valueTypes[index])
		{
			AddDiagnostic("Cannot assign value of different type to identifier: " + names[index]);
		}
	}
}

void SemanticAnalyzer::DefineShortVariables(const std::vector<std::string>& names, const std::vector<Type>& valueTypes)
{
	if (names.size() != valueTypes.size())
	{
		AddDiagnostic("Short variable declaration expects the same number of names and values.");
		return;
	}

	for (std::size_t index = 0; index < names.size(); ++index)
	{
		if (m_symbolTable.ResolveInCurrentScope(names[index]))
		{
			AddDiagnostic("Variable is already declared in current scope: " + names[index]);
			continue;
		}
		if (valueTypes[index] == Type::ERROR)
		{
			continue;
		}

		m_symbolTable.Define(SemanticSymbol{ names[index], valueTypes[index], SemanticSymbolKind::VARIABLE, {} });
	}
}

void SemanticAnalyzer::DefineVariables(
	const std::vector<std::string>& names,
	const std::optional<Type> declaredType,
	const std::vector<Type>& valueTypes)
{
	if (!valueTypes.empty() && names.size() != valueTypes.size())
	{
		AddDiagnostic("Variable declaration expects the same number of names and values.");
		return;
	}
	if (!declaredType.has_value() && valueTypes.empty())
	{
		AddDiagnostic("Variable declaration without type requires initializer.");
		return;
	}

	for (std::size_t index = 0; index < names.size(); ++index)
	{
		if (m_symbolTable.ResolveInCurrentScope(names[index]))
		{
			AddDiagnostic("Variable is already declared in current scope: " + names[index]);
			continue;
		}

		Type symbolType = declaredType.value_or(valueTypes[index]);
		if (!valueTypes.empty() && valueTypes[index] != Type::ERROR && symbolType != valueTypes[index])
		{
			AddDiagnostic("Variable initializer type does not match declared type for: " + names[index]);
			continue;
		}
		if (symbolType == Type::ERROR)
		{
			continue;
		}

		m_symbolTable.Define(SemanticSymbol{ names[index], symbolType, SemanticSymbolKind::VARIABLE, {} });
	}
}

void SemanticAnalyzer::Visit(const ProgramASTNode& node)
{
	PredeclareTopLevelFunctions(node.GetStatements());
	const std::size_t diagnosticCount = m_diagnostics.size();
	ValidateEntryPoint();
	const Type statementsType = AnalyzeChild(node.GetStatements());
	SetCurrentType(node, statementsType == Type::ERROR || m_diagnostics.size() != diagnosticCount ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const StatementListASTNode& node)
{
	bool hasChildError = false;
	bool hasReturned = false;
	for (const ASTNodePtr& child : node.GetStatements())
	{
		if (hasReturned)
		{
			AddDiagnostic("Unreachable statement.");
			hasChildError = true;
		}

		const Type childType = AnalyzeChild(*child);
		if (childType == Type::ERROR)
		{
			hasChildError = true;
		}
		if (AlwaysReturns(*child))
		{
			hasReturned = true;
		}
	}

	SetCurrentType(node, hasChildError ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const BlockASTNode& node)
{
	m_symbolTable.EnterScope();
	const Type statementsType = AnalyzeChild(node.GetStatements());
	m_symbolTable.ExitScope();
	SetCurrentType(node, statementsType == Type::ERROR ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const IfASTNode& node)
{
	const Type conditionType = AnalyzeChild(node.GetCondition());
	const Type thenType = AnalyzeChild(node.GetThenBranch());
	const ASTNode* elseBranch = node.GetElseBranch();
	const Type elseType = elseBranch ? AnalyzeChild(*elseBranch) : Type::VOID;

	bool hasError = false;
	if (conditionType == Type::ERROR)
	{
		hasError = true;
	}
	else if (!IsFalsey(conditionType))
	{
		AddDiagnostic("If condition expects truthy-compatible expression.");
		hasError = true;
	}
	if (thenType == Type::ERROR || elseType == Type::ERROR)
	{
		hasError = true;
	}

	SetCurrentType(node, hasError ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const PrintfASTNode& node)
{
	const Type argumentType = AnalyzeChild(node.GetArgument());
	if (argumentType == Type::ERROR)
	{
		SetCurrentType(node, Type::ERROR);
		return;
	}
	if (!IsFalsey(argumentType))
	{
		AddDiagnostic("printf expects int or bool argument.");
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetCurrentType(node, Type::VOID);
}

void SemanticAnalyzer::Visit(const ReturnASTNode& node)
{
	if (!m_currentFunctionReturnType.has_value())
	{
		if (const ASTNode* value = node.GetValue())
		{
			const Type valueType = AnalyzeChild(*value);
			if (valueType == Type::ERROR)
			{
				SetCurrentType(node, Type::ERROR);
				return;
			}
		}

		AddDiagnostic("Return statement is not allowed outside function.");
		SetCurrentType(node, Type::ERROR);
		return;
	}

	const Type expectedReturnType = *m_currentFunctionReturnType;
	if (const ASTNode* value = node.GetValue())
	{
		const Type valueType = AnalyzeChild(*value);
		if (valueType == Type::ERROR)
		{
			SetCurrentType(node, Type::ERROR);
			return;
		}
		if (expectedReturnType == Type::VOID)
		{
			AddDiagnostic("Void function cannot return a value.");
			SetCurrentType(node, Type::ERROR);
			return;
		}
		if (valueType != expectedReturnType)
		{
			AddDiagnostic("Return value type does not match function return type.");
			SetCurrentType(node, Type::ERROR);
			return;
		}

		SetCurrentType(node, Type::VOID);
		return;
	}

	if (expectedReturnType != Type::VOID)
	{
		AddDiagnostic("Non-void function must return a value.");
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetCurrentType(node, Type::VOID);
}

void SemanticAnalyzer::Visit(const FunctionDeclarationASTNode& node)
{
	if (!m_predeclaredFunctions.contains(&node) && !DefineFunctionSymbol(node))
	{
		SetCurrentType(node, Type::ERROR);
		return;
	}

	const std::optional<Type> previousReturnType = m_currentFunctionReturnType;
	m_currentFunctionReturnType = node.GetReturnType();
	m_symbolTable.EnterScope();
	bool hasParameterError = false;
	for (const FunctionParameter& parameter : node.GetParameters())
	{
		if (m_symbolTable.ResolveInCurrentScope(parameter.name))
		{
			AddDiagnostic("Function parameter is already declared: " + parameter.name);
			hasParameterError = true;
			continue;
		}

		m_symbolTable.Define(SemanticSymbol{ parameter.name, parameter.type, SemanticSymbolKind::VARIABLE, {} });
	}
	const Type bodyType = AnalyzeChild(node.GetBody());
	m_symbolTable.ExitScope();
	m_currentFunctionReturnType = previousReturnType;

	bool hasReturnError = false;
	if (node.GetReturnType() != Type::VOID && !AlwaysReturns(node.GetBody()))
	{
		AddDiagnostic("Non-void function must return a value on all execution paths: " + node.GetName());
		hasReturnError = true;
	}

	SetCurrentType(node, hasParameterError || hasReturnError || bodyType == Type::ERROR ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::PredeclareTopLevelFunctions(const ASTNode& node)
{
	const auto* statementList = dynamic_cast<const StatementListASTNode*>(&node);
	if (!statementList)
	{
		return;
	}

	PredeclareTopLevelFunctions(*statementList);
}

void SemanticAnalyzer::PredeclareTopLevelFunctions(const StatementListASTNode& node)
{
	for (const ASTNodePtr& child : node.GetStatements())
	{
		// TODO переделать без dynamic_cast
		const auto* function = dynamic_cast<const FunctionDeclarationASTNode*>(child.get());
		if (!function)
		{
			continue;
		}

		PredeclareFunction(*function);
	}
}

void SemanticAnalyzer::PredeclareFunction(const FunctionDeclarationASTNode& node)
{
	m_predeclaredFunctions.insert(&node);
	const bool wasDefined = DefineFunctionSymbol(node);
	(void)wasDefined;
}

void SemanticAnalyzer::ValidateEntryPoint()
{
	const SemanticSymbol* mainSymbol = m_symbolTable.ResolveInCurrentScope("main");
	if (!mainSymbol)
	{
		AddDiagnostic("Program entry point main is not declared.");
		return;
	}
	if (mainSymbol->kind != SemanticSymbolKind::FUNCTION)
	{
		AddDiagnostic("Program entry point main must be a function.");
		return;
	}
	if (!mainSymbol->parameterTypes.empty())
	{
		AddDiagnostic("Program entry point main must not have parameters.");
	}
	if (mainSymbol->type != Type::VOID)
	{
		AddDiagnostic("Program entry point main must return void.");
	}
}

bool SemanticAnalyzer::DefineFunctionSymbol(const FunctionDeclarationASTNode& node)
{
	if (m_symbolTable.ResolveInCurrentScope(node.GetName()))
	{
		AddDiagnostic("Function is already declared in current scope: " + node.GetName());
		return false;
	}

	m_symbolTable.Define(SemanticSymbol{
		node.GetName(),
		node.GetReturnType(),
		SemanticSymbolKind::FUNCTION,
		BuildParameterTypes(node) });
	return true;
}

std::vector<Type> SemanticAnalyzer::BuildParameterTypes(const FunctionDeclarationASTNode& node)
{
	std::vector<Type> parameterTypes;
	parameterTypes.reserve(node.GetParameters().size());
	for (const FunctionParameter& parameter : node.GetParameters())
	{
		parameterTypes.push_back(parameter.type);
	}
	return parameterTypes;
}

// TODO избавиться от dynamic_cast
bool SemanticAnalyzer::AlwaysReturns(const ASTNode& node)
{
	if (dynamic_cast<const ReturnASTNode*>(&node))
	{
		return true;
	}
	if (const auto* statementList = dynamic_cast<const StatementListASTNode*>(&node))
	{
		return StatementListAlwaysReturns(*statementList);
	}
	if (const auto* block = dynamic_cast<const BlockASTNode*>(&node))
	{
		return AlwaysReturns(block->GetStatements());
	}
	if (const auto* ifNode = dynamic_cast<const IfASTNode*>(&node))
	{
		return IfAlwaysReturns(*ifNode);
	}

	return false;
}

bool SemanticAnalyzer::StatementListAlwaysReturns(const StatementListASTNode& node)
{
	for (const ASTNodePtr& child : node.GetStatements())
	{
		if (AlwaysReturns(*child))
		{
			return true;
		}
	}

	return false;
}

bool SemanticAnalyzer::IfAlwaysReturns(const IfASTNode& node)
{
	const ASTNode* elseBranch = node.GetElseBranch();
	return elseBranch && AlwaysReturns(node.GetThenBranch()) && AlwaysReturns(*elseBranch);
}

Type SemanticAnalyzer::AnalyzeChild(const ASTNode& node)
{
	node.Accept(*this);
	return m_currentType;
}

std::vector<Type> SemanticAnalyzer::AnalyzeValues(const std::vector<ASTNodePtr>& values)
{
	std::vector<Type> result;
	result.reserve(values.size());
	for (const ASTNodePtr& value : values)
	{
		result.push_back(AnalyzeChild(*value));
	}
	return result;
}

// TODO избавиться от дублирования кода
bool SemanticAnalyzer::IsFalsey(const Type type)
{
	return type == Type::INT || type == Type::BOOL;
}

bool SemanticAnalyzer::HasError(const std::vector<Type>& types)
{
	for (const Type type : types)
	{
		if (type == Type::ERROR)
		{
			return true;
		}
	}
	return false;
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
