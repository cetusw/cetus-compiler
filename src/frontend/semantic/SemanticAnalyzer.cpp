#include "SemanticAnalyzer.h"

#include "rules/TypeRules.h"
#include "src/frontend/syntax/ast/ASTNode.h"

// TODO избавится от dynamic_cast
namespace
{
const IdentifierASTNode* GetAddressedIdentifier(const ASTNode& node)
{
	const auto* addressOf = dynamic_cast<const AddressOfASTNode*>(&node);
	if (!addressOf)
	{
		return nullptr;
	}
	return dynamic_cast<const IdentifierASTNode*>(&addressOf->GetTarget());
}

bool IsAddressOfExpression(const ASTNode& node)
{
	return dynamic_cast<const AddressOfASTNode*>(&node) != nullptr;
}
} // namespace

SemanticAnalyzer::SemanticAnalyzer(SymbolTable symbols)
	: m_symbolTable(std::move(symbols))
{
}

TypeCheckResult SemanticAnalyzer::Analyze(const ASTNode& node)
{
	m_currentType = Type::ERROR;
	m_diagnostics.clear();
	m_predeclaredFunctions.clear();
	m_predeclaredTypes.clear();
	DefineBuiltinFunctions();
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

	if (symbol->kind == SemanticSymbolKind::FUNCTION || symbol->kind == SemanticSymbolKind::BUILTIN_FUNCTION)
	{
		AddDiagnostic("Function identifier cannot be used as value: " + node.GetName());
		SetCurrentType(node, Type::ERROR);
		return;
	}
	if (symbol->kind == SemanticSymbolKind::TYPE)
	{
		AddDiagnostic("Type identifier cannot be used as value: " + node.GetName());
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetCurrentType(node, symbol->type);
}

void SemanticAnalyzer::Visit(const AddressOfASTNode& node)
{
	if (GetAddressedIdentifier(node) == nullptr)
	{
		AddDiagnostic("Address-of operator expects assignable identifier.");
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetCurrentType(node, AnalyzeChild(node.GetTarget()));
}

void SemanticAnalyzer::Visit(const UnaryASTNode& node)
{
	const TypeDescriptor operandType = AnalyzeChild(node.GetOperand());
	if (!ValidateValueExpression(operandType, "unary operand"))
	{
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetTypeCheckResult(node, TypeRules::CheckUnaryOperator(node.GetOperator(), operandType));
}

void SemanticAnalyzer::Visit(const BinaryASTNode& node)
{
	const TypeDescriptor leftType = AnalyzeChild(node.GetLeft());
	const TypeDescriptor rightType = AnalyzeChild(node.GetRight());
	bool hasValueError = false;
	if (!ValidateValueExpression(leftType, "binary operand"))
	{
		hasValueError = true;
	}
	if (!ValidateValueExpression(rightType, "binary operand"))
	{
		hasValueError = true;
	}
	if (hasValueError)
	{
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetTypeCheckResult(node, TypeRules::CheckBinaryOperator(node.GetOperator(), leftType, rightType));
}

void SemanticAnalyzer::Visit(const MemberAccessASTNode& node)
{
	const TypeDescriptor objectType = AnalyzeChild(node.GetObject());
	if (!ValidateValueExpression(objectType, "member access object"))
	{
		SetCurrentType(node, Type::ERROR);
		return;
	}

	const FieldSignature* field = ResolveField(objectType, node.GetMember());
	if (!field)
	{
		AddDiagnostic("Struct field is not declared: " + objectType.ToString() + "." + node.GetMember());
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetCurrentType(node, field->type);
}

void SemanticAnalyzer::Visit(const IndexASTNode& node)
{
	const TypeDescriptor objectType = AnalyzeChild(node.GetObject());
	const TypeDescriptor indexType = AnalyzeChild(node.GetIndex());
	bool hasError = false;

	if (!ValidateValueExpression(objectType, "indexed value"))
	{
		hasError = true;
	}
	else if (!objectType.IsSequence())
	{
		AddDiagnostic("Index access expects array or slice value.");
		hasError = true;
	}

	if (!ValidateValueExpression(indexType, "array index"))
	{
		hasError = true;
	}
	else if (indexType != Type::INT)
	{
		AddDiagnostic("Array index must have int type.");
		hasError = true;
	}

	SetCurrentType(node, hasError ? TypeDescriptor(Type::ERROR) : objectType.GetElementType());
}

// TODO отрефакторить
void SemanticAnalyzer::Visit(const CallExpressionASTNode& node)
{
	if (node.IsMethodCall())
	{
		const ASTNode* receiver = node.GetReceiver();
		if (!receiver)
		{
			AddDiagnostic("Method call receiver is missing: " + node.GetCalleeName());
			SetCurrentType(node, Type::ERROR);
			return;
		}

		const TypeDescriptor receiverType = AnalyzeChild(*receiver);
		if (!ValidateValueExpression(receiverType, "method receiver"))
		{
			SetCurrentType(node, Type::ERROR);
			return;
		}

		const MethodSignature* method = ResolveMethod(receiverType, node.GetCalleeName());
		if (!method)
		{
			AddDiagnostic("Method is not declared: " + receiverType.ToString() + "." + node.GetCalleeName());
			SetCurrentType(node, Type::ERROR);
			return;
		}

		TypeCheckMethodCall(node, receiverType, *method, AnalyzeValues(node.GetArguments()));
		return;
	}

	const SemanticSymbol* symbol = m_symbolTable.Resolve(node.GetCalleeName());
	if (!symbol)
	{
		AddDiagnostic("Undefined function: " + node.GetCalleeName());
		SetCurrentType(node, Type::ERROR);
		return;
	}
	if (!IsCallableKind(symbol->kind))
	{
		AddDiagnostic("Identifier is not a function: " + node.GetCalleeName());
		SetCurrentType(node, Type::ERROR);
		return;
	}

	const std::vector<TypeDescriptor> argumentTypes = AnalyzeValues(node.GetArguments());
	if (symbol->kind == SemanticSymbolKind::BUILTIN_FUNCTION)
	{
		TypeCheckBuiltinCall(node, argumentTypes);
		return;
	}

	TypeCheckFunctionCall(node, *symbol, argumentTypes);
}

void SemanticAnalyzer::TypeCheckMethodCall(
	const CallExpressionASTNode& node,
	const TypeDescriptor& receiverType,
	const MethodSignature& method,
	const std::vector<TypeDescriptor>& argumentTypes)
{
	bool hasError = false;
	if (method.receiverIsPointer)
	{
		const ASTNode* receiver = node.GetReceiver();
		if (!receiver || dynamic_cast<const IdentifierASTNode*>(receiver) == nullptr)
		{
			AddDiagnostic("Pointer method receiver expects assignable identifier receiver: " + receiverType.ToString() + "." + node.GetCalleeName());
			hasError = true;
		}
	}
	if (method.parameters.size() != argumentTypes.size())
	{
		AddDiagnostic("Method call argument count does not match method parameters: " + receiverType.ToString() + "." + node.GetCalleeName());
		SetCurrentType(node, Type::ERROR);
		return;
	}

	for (std::size_t index = 0; index < argumentTypes.size(); ++index)
	{
		if (!ValidateValueExpression(argumentTypes[index], "method argument"))
		{
			hasError = true;
			continue;
		}
		const ParameterSignature& parameter = method.parameters[index];
		if (argumentTypes[index] != parameter.type)
		{
			AddDiagnostic("Method call argument type does not match parameter type: " + receiverType.ToString() + "." + node.GetCalleeName());
			hasError = true;
		}
		if (parameter.isPointer)
		{
			AddDiagnostic("Pointer method parameter is not supported yet: " + receiverType.ToString() + "." + node.GetCalleeName());
			hasError = true;
		}
		if (IsAddressOfExpression(*node.GetArguments()[index]))
		{
			AddDiagnostic("Method call expects value argument, not address argument: " + receiverType.ToString() + "." + node.GetCalleeName());
			hasError = true;
		}
	}

	SetCurrentType(node, hasError ? Type::ERROR : method.type);
}

// TODO to refactor
void SemanticAnalyzer::TypeCheckBuiltinCall(const CallExpressionASTNode& node, const std::vector<TypeDescriptor>& argumentTypes)
{
	const std::string& calleeName = node.GetCalleeName();
	if (calleeName == "printf" || calleeName == "print" || calleeName == "println")
	{
		if (argumentTypes.size() != 1)
		{
			AddDiagnostic(calleeName + " expects exactly one argument.");
			SetCurrentType(node, Type::ERROR);
			return;
		}
		if (!ValidateValueExpression(argumentTypes.front(), "function argument"))
		{
			SetCurrentType(node, Type::ERROR);
			return;
		}
		if (IsAddressOfExpression(*node.GetArguments().front()))
		{
			AddDiagnostic(calleeName + " expects value argument, not address argument.");
			SetCurrentType(node, Type::ERROR);
			return;
		}
		const TypeDescriptor& argumentType = argumentTypes.front();
		if (argumentType != Type::INT
			&& argumentType != Type::FLOAT
			&& argumentType != Type::BOOL
			&& argumentType != Type::STRING)
		{
			AddDiagnostic(calleeName + " expects int, float, bool or string argument.");
			SetCurrentType(node, Type::ERROR);
			return;
		}

		SetCurrentType(node, Type::VOID);
		return;
	}

	if (calleeName == "len")
	{
		if (argumentTypes.size() != 1)
		{
			AddDiagnostic("len expects exactly one argument.");
			SetCurrentType(node, Type::ERROR);
			return;
		}
		if (!ValidateValueExpression(argumentTypes.front(), "function argument"))
		{
			SetCurrentType(node, Type::ERROR);
			return;
		}
		if (IsAddressOfExpression(*node.GetArguments().front()))
		{
			AddDiagnostic("len expects value argument, not address argument.");
			SetCurrentType(node, Type::ERROR);
			return;
		}
		if (!argumentTypes.front().IsIndexable())
		{
			AddDiagnostic("len expects string, array or slice argument.");
			SetCurrentType(node, Type::ERROR);
			return;
		}

		SetCurrentType(node, Type::INT);
		return;
	}

	if (calleeName == "scan")
	{
		if (argumentTypes.size() != 1)
		{
			AddDiagnostic("scan expects exactly one argument.");
			SetCurrentType(node, Type::ERROR);
			return;
		}
		if (GetAddressedIdentifier(*node.GetArguments().front()) == nullptr)
		{
			AddDiagnostic("scan expects address of assignable identifier argument.");
			SetCurrentType(node, Type::ERROR);
			return;
		}
		if (!ValidateValueExpression(argumentTypes.front(), "function argument"))
		{
			SetCurrentType(node, Type::ERROR);
			return;
		}

		SetCurrentType(node, Type::VOID);
		return;
	}

	AddDiagnostic("Unsupported builtin function: " + node.GetCalleeName());
	SetCurrentType(node, Type::ERROR);
}

bool SemanticAnalyzer::ValidateValueExpression(const TypeDescriptor& type, const char* context)
{
	if (type == Type::ERROR)
	{
		return false;
	}
	if (type == Type::VOID)
	{
		AddDiagnostic(std::string("Void expression cannot be used as ") + context + ".");
		return false;
	}

	return true;
}

bool SemanticAnalyzer::ValidateTypeReference(const TypeDescriptor& type, const char* context)
{
	if (type == Type::ERROR)
	{
		return false;
	}
	if (type.IsSequence())
	{
		return ValidateTypeReference(type.GetElementType(), context);
	}
	if (!type.IsNamed())
	{
		return true;
	}

	const SemanticSymbol* symbol = m_symbolTable.Resolve(type.GetName());
	if (!symbol || symbol->kind != SemanticSymbolKind::TYPE)
	{
		AddDiagnostic(std::string("Unknown type in ") + context + ": " + type.GetName());
		return false;
	}

	return true;
}

bool SemanticAnalyzer::ValidateStructFields(const StructDeclarationASTNode& node)
{
	bool hasError = false;
	std::unordered_set<std::string> fieldNames;
	for (const StructField& field : node.GetFields())
	{
		if (!fieldNames.insert(field.name).second)
		{
			AddDiagnostic("Struct field is already declared: " + node.GetName() + "." + field.name);
			hasError = true;
		}
		if (!ValidateTypeReference(field.type, "struct field"))
		{
			hasError = true;
		}
	}

	return !hasError;
}

const FieldSignature* SemanticAnalyzer::ResolveField(const TypeDescriptor& objectType, const std::string& fieldName) const
{
	if (!objectType.IsNamed())
	{
		return nullptr;
	}

	const SemanticSymbol* symbol = m_symbolTable.Resolve(objectType.GetName());
	if (!symbol || symbol->kind != SemanticSymbolKind::TYPE)
	{
		return nullptr;
	}

	for (const FieldSignature& field : symbol->fields)
	{
		if (field.name == fieldName)
		{
			return &field;
		}
	}

	return nullptr;
}

const MethodSignature* SemanticAnalyzer::ResolveMethod(const TypeDescriptor& objectType, const std::string& methodName) const
{
	if (!objectType.IsNamed())
	{
		return nullptr;
	}

	const SemanticSymbol* symbol = m_symbolTable.Resolve(objectType.GetName());
	if (!symbol || symbol->kind != SemanticSymbolKind::TYPE)
	{
		return nullptr;
	}

	for (const MethodSignature& method : symbol->methods)
	{
		if (method.name == methodName)
		{
			return &method;
		}
	}

	return nullptr;
}

bool SemanticAnalyzer::ValidateUserDefinedName(const std::string& name, const char* declarationKind)
{
	const SemanticSymbol* existing = m_symbolTable.Resolve(name);
	if (existing && IsCallableKind(existing->kind))
	{
		AddDiagnostic(std::string(declarationKind) + " cannot use reserved callable identifier: " + name);
		return false;
	}
	if (existing && existing->kind == SemanticSymbolKind::TYPE)
	{
		AddDiagnostic(std::string(declarationKind) + " cannot use reserved type identifier: " + name);
		return false;
	}

	return true;
}

void SemanticAnalyzer::TypeCheckFunctionCall(
	const CallExpressionASTNode& node,
	const SemanticSymbol& symbol,
	const std::vector<TypeDescriptor>& argumentTypes)
{
	bool hasError = false;
	if (symbol.parameters.size() != argumentTypes.size())
	{
		AddDiagnostic("Function call argument count does not match function parameters: " + node.GetCalleeName());
		SetCurrentType(node, Type::ERROR);
		return;
	}

	for (std::size_t index = 0; index < argumentTypes.size(); ++index)
	{
		if (!ValidateValueExpression(argumentTypes[index], "function argument"))
		{
			hasError = true;
			continue;
		}
		const ParameterSignature& parameter = symbol.parameters[index];
		if (argumentTypes[index] != parameter.type)
		{
			AddDiagnostic("Function call argument type does not match parameter type: " + node.GetCalleeName());
			hasError = true;
		}
		const bool isAddressArgument = IsAddressOfExpression(*node.GetArguments()[index]);
		if (parameter.isPointer && GetAddressedIdentifier(*node.GetArguments()[index]) == nullptr)
		{
			AddDiagnostic("Pointer parameter expects address of assignable identifier argument: " + node.GetCalleeName());
			hasError = true;
		}
		if (!parameter.isPointer && isAddressArgument)
		{
			AddDiagnostic("Address argument requires pointer parameter: " + node.GetCalleeName());
			hasError = true;
		}
	}

	SetCurrentType(node, hasError ? Type::ERROR : symbol.type);
}

void SemanticAnalyzer::Visit(const AssignmentASTNode& node)
{
	const std::size_t diagnosticCount = m_diagnostics.size();
	const std::vector<TypeDescriptor> valueTypes = AnalyzeValues(node.GetValues());
	ValidateAssignment(node.GetTargets(), valueTypes);
	SetCurrentType(node, HasError(valueTypes) || m_diagnostics.size() != diagnosticCount ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const IncrementASTNode& node)
{
	const TypeDescriptor targetType = AnalyzeAssignmentTarget(node.GetTarget());
	if (targetType == Type::ERROR)
	{
		SetCurrentType(node, Type::ERROR);
		return;
	}
	if (targetType != Type::INT)
	{
		AddDiagnostic("Increment target must have int type.");
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetCurrentType(node, Type::VOID);
}

void SemanticAnalyzer::Visit(const DecrementASTNode& node)
{
	const TypeDescriptor targetType = AnalyzeAssignmentTarget(node.GetTarget());
	if (targetType == Type::ERROR)
	{
		SetCurrentType(node, Type::ERROR);
		return;
	}
	if (targetType != Type::INT)
	{
		AddDiagnostic("Decrement target must have int type.");
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetCurrentType(node, Type::VOID);
}

void SemanticAnalyzer::Visit(const ShortVariableDeclarationASTNode& node)
{
	const std::size_t diagnosticCount = m_diagnostics.size();
	const std::vector<TypeDescriptor> valueTypes = AnalyzeValues(node.GetValues());
	DefineShortVariables(node.GetNames(), valueTypes);
	SetCurrentType(node, HasError(valueTypes) || m_diagnostics.size() != diagnosticCount ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const VariableDeclarationASTNode& node)
{
	const std::size_t diagnosticCount = m_diagnostics.size();
	const std::vector<TypeDescriptor> valueTypes = AnalyzeValues(node.GetValues());
	DefineVariables(node.GetNames(), node.GetDeclaredType(), valueTypes);
	SetCurrentType(node, HasError(valueTypes) || m_diagnostics.size() != diagnosticCount ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const ExpressionStatementASTNode& node)
{
	const TypeDescriptor expressionType = AnalyzeChild(node.GetExpression());
	SetCurrentType(node, expressionType == Type::ERROR ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::ValidateAssignment(const std::vector<ASTNodePtr>& targets, const std::vector<TypeDescriptor>& valueTypes)
{
	if (targets.size() != valueTypes.size())
	{
		AddDiagnostic("Assignment expects the same number of targets and values.");
		return;
	}

	for (std::size_t index = 0; index < targets.size(); ++index)
	{
		if (!ValidateValueExpression(valueTypes[index], "assignment value"))
		{
			continue;
		}

		const TypeDescriptor targetType = AnalyzeAssignmentTarget(*targets[index]);
		if (targetType == Type::ERROR)
		{
			continue;
		}
		if (targetType != valueTypes[index])
		{
			AddDiagnostic("Cannot assign value of different type to assignment target.");
		}
	}
}

TypeDescriptor SemanticAnalyzer::AnalyzeAssignmentTarget(const ASTNode& target)
{
	if (const auto* identifier = dynamic_cast<const IdentifierASTNode*>(&target))
	{
		const SemanticSymbol* existing = m_symbolTable.Resolve(identifier->GetName());
		if (!existing)
		{
			AddDiagnostic("Cannot assign to undefined identifier: " + identifier->GetName());
			return Type::ERROR;
		}
		if (existing->kind != SemanticSymbolKind::VARIABLE)
		{
			AddDiagnostic("Cannot assign to non-variable identifier: " + identifier->GetName());
			return Type::ERROR;
		}
		target.SetInferredType(existing->type);
		return existing->type;
	}

	if (dynamic_cast<const IndexASTNode*>(&target) || dynamic_cast<const MemberAccessASTNode*>(&target))
	{
		return AnalyzeChild(target);
	}

	AddDiagnostic("Invalid assignment target.");
	return Type::ERROR;
}

void SemanticAnalyzer::DefineShortVariables(const std::vector<std::string>& names, const std::vector<TypeDescriptor>& valueTypes)
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
		if (!ValidateUserDefinedName(names[index], "Variable"))
		{
			continue;
		}
		if (!ValidateValueExpression(valueTypes[index], "variable initializer"))
		{
			continue;
		}

		m_symbolTable.Define(SemanticSymbol{ names[index], valueTypes[index], SemanticSymbolKind::VARIABLE, {}, {}, {} });
	}
}

void SemanticAnalyzer::DefineVariables(
	const std::vector<std::string>& names,
	const std::optional<TypeDescriptor>& declaredType,
	const std::vector<TypeDescriptor>& valueTypes)
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
		if (!ValidateUserDefinedName(names[index], "Variable"))
		{
			continue;
		}
		if (declaredType.has_value() && !ValidateTypeReference(*declaredType, "variable declaration"))
		{
			continue;
		}

		if (!valueTypes.empty() && !ValidateValueExpression(valueTypes[index], "variable initializer"))
		{
			continue;
		}
		const TypeDescriptor symbolType = declaredType.has_value() ? *declaredType : valueTypes[index];
		if (!valueTypes.empty() && symbolType != valueTypes[index])
		{
			AddDiagnostic("Variable initializer type does not match declared type for: " + names[index]);
			continue;
		}
		if (symbolType == Type::ERROR)
		{
			continue;
		}

		m_symbolTable.Define(SemanticSymbol{ names[index], symbolType, SemanticSymbolKind::VARIABLE, {}, {}, {} });
	}
}

void SemanticAnalyzer::Visit(const ProgramASTNode& node)
{
	PredeclareTopLevelTypes(node.GetStatements());
	PredeclareTopLevelFunctions(node.GetStatements());
	const std::size_t diagnosticCount = m_diagnostics.size();
	ValidateEntryPoint();
	const TypeDescriptor statementsType = AnalyzeChild(node.GetStatements());
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

		const TypeDescriptor childType = AnalyzeChild(*child);
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
	const TypeDescriptor statementsType = AnalyzeChild(node.GetStatements());
	m_symbolTable.ExitScope();
	SetCurrentType(node, statementsType == Type::ERROR ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const IfASTNode& node)
{
	const TypeDescriptor conditionType = AnalyzeChild(node.GetCondition());
	const TypeDescriptor thenType = AnalyzeChild(node.GetThenBranch());
	const ASTNode* elseBranch = node.GetElseBranch();
	const TypeDescriptor elseType = elseBranch ? AnalyzeChild(*elseBranch) : Type::VOID;

	bool hasError = false;
	if (!ValidateValueExpression(conditionType, "condition"))
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

void SemanticAnalyzer::Visit(const ForASTNode& node)
{
	bool hasError = false;
	if (const ASTNode* initializer = node.GetInitializer())
	{
		m_symbolTable.EnterScope();
		const TypeDescriptor initializerType = AnalyzeChild(*initializer);
		if (initializerType == Type::ERROR)
		{
			hasError = true;
		}
	}

	const TypeDescriptor conditionType = AnalyzeChild(node.GetCondition());
	if (!ValidateValueExpression(conditionType, "for condition"))
	{
		hasError = true;
	}
	else if (!IsFalsey(conditionType))
	{
		AddDiagnostic("For condition expects truthy-compatible expression.");
		hasError = true;
	}

	if (const ASTNode* post = node.GetPost())
	{
		if (dynamic_cast<const ShortVariableDeclarationASTNode*>(post)
			|| dynamic_cast<const VariableDeclarationASTNode*>(post))
		{
			AddDiagnostic("For post statement cannot declare variables.");
			hasError = true;
		}

		const TypeDescriptor postType = AnalyzeChild(*post);
		if (postType == Type::ERROR)
		{
			hasError = true;
		}
	}

	++m_loopDepth;
	const TypeDescriptor bodyType = AnalyzeChild(node.GetBody());
	--m_loopDepth;
	if (bodyType == Type::ERROR)
	{
		hasError = true;
	}

	if (node.GetInitializer())
	{
		m_symbolTable.ExitScope();
	}

	SetCurrentType(node, hasError ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const BreakASTNode& node)
{
	if (m_loopDepth == 0)
	{
		AddDiagnostic("Break statement is not allowed outside loop.");
		SetCurrentType(node, Type::ERROR);
		return;
	}

	SetCurrentType(node, Type::VOID);
}

void SemanticAnalyzer::Visit(const ContinueASTNode& node)
{
	if (m_loopDepth == 0)
	{
		AddDiagnostic("Continue statement is not allowed outside loop.");
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
			const TypeDescriptor valueType = AnalyzeChild(*value);
			if (!ValidateValueExpression(valueType, "return value"))
			{
				SetCurrentType(node, Type::ERROR);
				return;
			}
		}

		AddDiagnostic("Return statement is not allowed outside function.");
		SetCurrentType(node, Type::ERROR);
		return;
	}

	const TypeDescriptor expectedReturnType = *m_currentFunctionReturnType;
	if (const ASTNode* value = node.GetValue())
	{
		const TypeDescriptor valueType = AnalyzeChild(*value);
		if (!ValidateValueExpression(valueType, "return value"))
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

	const std::optional<TypeDescriptor> previousReturnType = m_currentFunctionReturnType;
	m_currentFunctionReturnType = node.GetReturnType();
	bool hasSignatureError = false;
	if (!ValidateTypeReference(node.GetReturnType(), "function return type"))
	{
		hasSignatureError = true;
	}
	m_symbolTable.EnterScope();
	bool hasParameterError = false;
	if (const FunctionParameter* receiver = node.GetReceiver())
	{
		if (!receiver->type.IsNamed() || !ValidateTypeReference(receiver->type, "method receiver"))
		{
			AddDiagnostic("Method receiver must use declared struct type: " + node.GetName());
			hasParameterError = true;
		}
		if (m_symbolTable.ResolveInCurrentScope(receiver->name))
		{
			AddDiagnostic("Method receiver is already declared: " + receiver->name);
			hasParameterError = true;
		}
		else if (!ValidateUserDefinedName(receiver->name, "Method receiver"))
		{
			hasParameterError = true;
		}
		else
		{
			m_symbolTable.Define(SemanticSymbol{ receiver->name, receiver->type, SemanticSymbolKind::VARIABLE, {}, {}, {} });
		}
	}
	for (const FunctionParameter& parameter : node.GetParameters())
	{
		if (!ValidateTypeReference(parameter.type, "function parameter"))
		{
			hasParameterError = true;
			continue;
		}
		if (m_symbolTable.ResolveInCurrentScope(parameter.name))
		{
			AddDiagnostic("Function parameter is already declared: " + parameter.name);
			hasParameterError = true;
			continue;
		}
		if (!ValidateUserDefinedName(parameter.name, "Function parameter"))
		{
			hasParameterError = true;
			continue;
		}

		m_symbolTable.Define(SemanticSymbol{ parameter.name, parameter.type, SemanticSymbolKind::VARIABLE, {}, {}, {} });
	}
	const TypeDescriptor bodyType = AnalyzeChild(node.GetBody());
	m_symbolTable.ExitScope();
	m_currentFunctionReturnType = previousReturnType;

	bool hasReturnError = false;
	if (node.GetReturnType() != Type::VOID && !AlwaysReturns(node.GetBody()))
	{
		AddDiagnostic("Non-void function must return a value on all execution paths: " + node.GetQualifiedName());
		hasReturnError = true;
	}

	SetCurrentType(node, hasSignatureError || hasParameterError || hasReturnError || bodyType == Type::ERROR ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::Visit(const StructDeclarationASTNode& node)
{
	bool hasError = false;
	if (!m_predeclaredTypes.contains(&node) && !DefineTypeSymbol(node))
	{
		hasError = true;
	}
	if (!ValidateStructFields(node))
	{
		hasError = true;
	}

	SetCurrentType(node, hasError ? Type::ERROR : Type::VOID);
}

void SemanticAnalyzer::PredeclareTopLevelTypes(const ASTNode& node)
{
	const auto* statementList = dynamic_cast<const StatementListASTNode*>(&node);
	if (!statementList)
	{
		return;
	}

	PredeclareTopLevelTypes(*statementList);
}

void SemanticAnalyzer::PredeclareTopLevelTypes(const StatementListASTNode& node)
{
	for (const ASTNodePtr& child : node.GetStatements())
	{
		// TODO переделать без dynamic_cast
		const auto* typeDeclaration = dynamic_cast<const StructDeclarationASTNode*>(child.get());
		if (!typeDeclaration)
		{
			continue;
		}

		m_predeclaredTypes.insert(typeDeclaration);
		const bool wasDefined = DefineTypeSymbol(*typeDeclaration);
		(void)wasDefined;
	}
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
	if (!mainSymbol->parameters.empty())
	{
		AddDiagnostic("Program entry point main must not have parameters.");
	}
	if (mainSymbol->type != Type::VOID)
	{
		AddDiagnostic("Program entry point main must return void.");
	}
}

void SemanticAnalyzer::DefineBuiltinFunctions()
{
	if (m_symbolTable.ResolveInCurrentScope("printf"))
	{
		return;
	}

	m_symbolTable.Define(SemanticSymbol{ "printf", Type::VOID, SemanticSymbolKind::BUILTIN_FUNCTION, { ParameterSignature{ Type::ERROR, false } }, {}, {} });
	m_symbolTable.Define(SemanticSymbol{ "print", Type::VOID, SemanticSymbolKind::BUILTIN_FUNCTION, { ParameterSignature{ Type::ERROR, false } }, {}, {} });
	m_symbolTable.Define(SemanticSymbol{ "println", Type::VOID, SemanticSymbolKind::BUILTIN_FUNCTION, { ParameterSignature{ Type::ERROR, false } }, {}, {} });
	m_symbolTable.Define(SemanticSymbol{ "len", Type::INT, SemanticSymbolKind::BUILTIN_FUNCTION, { ParameterSignature{ Type::STRING, false } }, {}, {} });
	m_symbolTable.Define(SemanticSymbol{ "scan", Type::VOID, SemanticSymbolKind::BUILTIN_FUNCTION, { ParameterSignature{ Type::ERROR, true } }, {}, {} });
}

bool SemanticAnalyzer::DefineFunctionSymbol(const FunctionDeclarationASTNode& node)
{
	const std::string symbolName = node.GetQualifiedName();
	const SemanticSymbol* existing = m_symbolTable.ResolveInCurrentScope(symbolName);
	if (existing && existing->kind == SemanticSymbolKind::BUILTIN_FUNCTION)
	{
		return ValidateUserDefinedName(symbolName, node.IsMethod() ? "Method" : "Function");
	}
	if (existing)
	{
		AddDiagnostic(std::string(node.IsMethod() ? "Method" : "Function") + " is already declared in current scope: " + symbolName);
		return false;
	}
	if (!node.IsMethod() && !ValidateUserDefinedName(node.GetName(), "Function"))
	{
		return false;
	}
	if (const FunctionParameter* receiver = node.GetReceiver())
	{
		if (!receiver->type.IsNamed())
		{
			AddDiagnostic("Method receiver must use declared struct type: " + node.GetName());
			return false;
		}

		SemanticSymbol* typeSymbol = m_symbolTable.ResolveMutableInCurrentScope(receiver->type.GetName());
		if (!typeSymbol || typeSymbol->kind != SemanticSymbolKind::TYPE)
		{
			AddDiagnostic("Unknown method receiver type: " + receiver->type.ToString());
			return false;
		}
		for (const MethodSignature& method : typeSymbol->methods)
		{
			if (method.name == node.GetName())
			{
				AddDiagnostic("Method is already declared for type: " + symbolName);
				return false;
			}
		}
		typeSymbol->methods.push_back(MethodSignature{
			node.GetName(),
			symbolName,
			node.GetReturnType(),
			receiver->isPointer,
			BuildParameterSignatures(node) });
	}

	m_symbolTable.Define(SemanticSymbol{
		symbolName,
		node.GetReturnType(),
		SemanticSymbolKind::FUNCTION,
		BuildCallableParameterSignatures(node),
		{},
		{} });
	return true;
}

bool SemanticAnalyzer::DefineTypeSymbol(const StructDeclarationASTNode& node)
{
	const SemanticSymbol* existing = m_symbolTable.ResolveInCurrentScope(node.GetName());
	if (existing)
	{
		AddDiagnostic("Type is already declared in current scope: " + node.GetName());
		return false;
	}
	if (!ValidateUserDefinedName(node.GetName(), "Type"))
	{
		return false;
	}

	m_symbolTable.Define(SemanticSymbol{
		node.GetName(),
		TypeDescriptor::Named(node.GetName()),
		SemanticSymbolKind::TYPE,
		{},
		BuildFieldSignatures(node),
		{} });
	return true;
}

std::vector<ParameterSignature> SemanticAnalyzer::BuildParameterSignatures(const FunctionDeclarationASTNode& node)
{
	std::vector<ParameterSignature> parameterSignatures;
	parameterSignatures.reserve(node.GetParameters().size());
	for (const FunctionParameter& parameter : node.GetParameters())
	{
		parameterSignatures.push_back(ParameterSignature{ parameter.type, parameter.isPointer });
	}
	return parameterSignatures;
}

std::vector<ParameterSignature> SemanticAnalyzer::BuildCallableParameterSignatures(const FunctionDeclarationASTNode& node)
{
	std::vector<ParameterSignature> parameterSignatures;
	parameterSignatures.reserve(node.GetParameters().size() + (node.GetReceiver() ? 1 : 0));
	if (const FunctionParameter* receiver = node.GetReceiver())
	{
		parameterSignatures.push_back(ParameterSignature{ receiver->type, receiver->isPointer });
	}
	for (const FunctionParameter& parameter : node.GetParameters())
	{
		parameterSignatures.push_back(ParameterSignature{ parameter.type, parameter.isPointer });
	}
	return parameterSignatures;
}

std::vector<FieldSignature> SemanticAnalyzer::BuildFieldSignatures(const StructDeclarationASTNode& node)
{
	std::vector<FieldSignature> fieldSignatures;
	fieldSignatures.reserve(node.GetFields().size());
	for (const StructField& field : node.GetFields())
	{
		fieldSignatures.push_back(FieldSignature{ field.name, field.type });
	}
	return fieldSignatures;
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

bool SemanticAnalyzer::IsCallableKind(const SemanticSymbolKind kind)
{
	return kind == SemanticSymbolKind::FUNCTION || kind == SemanticSymbolKind::BUILTIN_FUNCTION;
}

TypeDescriptor SemanticAnalyzer::AnalyzeChild(const ASTNode& node)
{
	node.Accept(*this);
	return m_currentType;
}

std::vector<TypeDescriptor> SemanticAnalyzer::AnalyzeValues(const std::vector<ASTNodePtr>& values)
{
	std::vector<TypeDescriptor> result;
	result.reserve(values.size());
	for (const ASTNodePtr& value : values)
	{
		result.push_back(AnalyzeChild(*value));
	}
	return result;
}

// TODO избавиться от дублирования кода
bool SemanticAnalyzer::IsFalsey(TypeDescriptor type)
{
	return type == Type::INT || type == Type::BOOL;
}

bool SemanticAnalyzer::HasError(const std::vector<TypeDescriptor>& types)
{
	for (const TypeDescriptor& type : types)
	{
		if (type == Type::ERROR)
		{
			return true;
		}
	}
	return false;
}

void SemanticAnalyzer::SetCurrentType(const ASTNode& node, const TypeDescriptor& type)
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
