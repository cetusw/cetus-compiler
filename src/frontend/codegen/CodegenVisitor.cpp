#include "CodegenVisitor.h"

#include "OperatorOpcodeResolver.h"
#include "src/backend/vm/objects/ObjFunction.h"
#include "src/backend/vm/objects/ObjString.h"
#include "src/frontend/syntax/ast/ASTNode.h"

namespace
{
constexpr auto PRINTF_NATIVE_NAME = "println";

// TODO избавиться от дубликата
const IdentifierASTNode* GetAddressedIdentifier(const ASTNode& node)
{
	const auto* addressOf = dynamic_cast<const AddressOfASTNode*>(&node);
	if (!addressOf)
	{
		return nullptr;
	}
	return dynamic_cast<const IdentifierASTNode*>(&addressOf->GetTarget());
}

std::string ResolveBuiltinRuntimeName(const std::string& sourceName)
{
	if (sourceName == "printf")
	{
		return PRINTF_NATIVE_NAME;
	}
	return sourceName;
}
}

CodegenVisitor::CodegenVisitor(const SymbolTable& symbols, const TypeCheckResult& typeInfo)
	: m_symbols(symbols)
	, m_typeInfo(typeInfo)
{
}

CodegenResult CodegenVisitor::Generate(const ASTNode& expr)
{
	m_programContext.Reset();
	m_functionStack.clear();
	m_error.reset();

	if (!EnsureTyped(expr))
	{
		return CodegenResult::Error(*m_error);
	}

	expr.Accept(*this);
	if (m_error.has_value())
	{
		return CodegenResult::Error(*m_error);
	}

	if (!m_programContext.HasEntryPoint())
	{
		return CodegenResult::Error("Program entry point main was not generated.");
	}

	return CodegenResult::Success(m_programContext.Build());
}

void CodegenVisitor::Visit(const BoolLiteralASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	CurrentEmitter().EmitConstant(Value(expr.GetValue()));
}

void CodegenVisitor::Visit(const IntLiteralASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	CurrentEmitter().EmitConstant(Value(static_cast<RuntimeInt>(std::stoll(expr.GetValue()))));
}

void CodegenVisitor::Visit(const FloatLiteralASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	CurrentEmitter().EmitConstant(Value(std::stod(expr.GetValue())));
}

void CodegenVisitor::Visit(const StringLiteralASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	CurrentEmitter().EmitConstant(Value(std::make_shared<ObjString>(expr.GetValue())));
}

void CodegenVisitor::Visit(const IdentifierASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	if (const std::optional<int> localSlot = m_functionStack.back().ResolveLocal(expr.GetName()))
	{
		CurrentEmitter().EmitLocalLoad(*localSlot);
		return;
	}

	CurrentEmitter().EmitGlobalLoad(expr.GetName());
}

void CodegenVisitor::Visit(const AddressOfASTNode&)
{
	Fail("Address-of expression can only be used as argument for pointer parameter.");
}

void CodegenVisitor::Visit(const UnaryASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	expr.GetOperand().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	const std::optional<OpCode> opcode = OperatorOpcodeResolver::Resolve(expr.GetOperator());
	if (!opcode.has_value())
	{
		Fail("Unsupported unary operator during code generation.");
		return;
	}

	CurrentEmitter().EmitOpcode(*opcode);
}

void CodegenVisitor::Visit(const BinaryASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	if (expr.GetOperator() == BinaryOperator::AND)
	{
		EmitLogicalAnd(expr);
		return;
	}
	if (expr.GetOperator() == BinaryOperator::OR)
	{
		EmitLogicalOr(expr);
		return;
	}

	expr.GetLeft().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	expr.GetRight().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	EmitBinaryOperation(expr.GetOperator());
}

void CodegenVisitor::Visit(const MemberAccessASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	expr.GetObject().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	CurrentEmitter().EmitMemberLoad(expr.GetMember());
}

void CodegenVisitor::Visit(const IndexASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	expr.GetObject().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	expr.GetIndex().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	CurrentEmitter().EmitIndexLoad();
}

void CodegenVisitor::Visit(const CallExpressionASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	if (expr.IsMethodCall())
	{
		const ASTNode* receiver = expr.GetReceiver();
		if (!receiver || !receiver->GetInferredType().has_value())
		{
			Fail("Method call receiver type is missing during code generation.");
			return;
		}

		const std::string targetName = receiver->GetInferredType()->ToString() + "." + expr.GetCalleeName();
		const MethodSignature* method = ResolveMethod(*receiver->GetInferredType(), expr.GetCalleeName());
		if (!method)
		{
			Fail("Method symbol is missing during code generation: " + targetName);
			return;
		}

		CurrentEmitter().EmitGlobalLoad(targetName);
		if (method->receiverIsPointer)
		{
			const auto* identifier = dynamic_cast<const IdentifierASTNode*>(receiver);
			if (!identifier)
			{
				Fail("Pointer method receiver code generation expects identifier receiver.");
				return;
			}
			EmitIdentifierRef(*identifier);
		}
		else
		{
			receiver->Accept(*this);
		}
		if (m_error.has_value())
		{
			return;
		}

		const std::vector<ASTNodePtr>& arguments = expr.GetArguments();
		for (const ASTNodePtr& argument : arguments)
		{
			argument->Accept(*this);
			if (m_error.has_value())
			{
				return;
			}
		}

		CurrentEmitter().EmitOpcode(OP_CALL);
		CurrentEmitter().EmitOperandByte(static_cast<int>(arguments.size() + 1));
		return;
	}

	const std::string& calleeName = expr.GetCalleeName();
	const std::string targetName = ResolveBuiltinRuntimeName(calleeName);
	CurrentEmitter().EmitGlobalLoad(targetName);

	const std::vector<ASTNodePtr>& arguments = expr.GetArguments();
	for (std::size_t index = 0; index < arguments.size(); ++index)
	{
		if (ShouldPassArgumentByPointer(calleeName, index))
		{
			const IdentifierASTNode* identifier = GetAddressedIdentifier(*arguments[index]);
			if (!identifier)
			{
				Fail("Pointer argument code generation expects address of identifier.");
				return;
			}
			EmitIdentifierRef(*identifier);
		}
		else
		{
			arguments[index]->Accept(*this);
		}
		if (m_error.has_value())
		{
			return;
		}
	}

	CurrentEmitter().EmitOpcode(OP_CALL);
	CurrentEmitter().EmitOperandByte(static_cast<int>(expr.GetArguments().size()));
}

void CodegenVisitor::Visit(const AssignmentASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	const std::vector<ASTNodePtr>& targets = expr.GetTargets();
	const std::vector<ASTNodePtr>& values = expr.GetValues();

	for (const ASTNodePtr& value : values)
	{
		value->Accept(*this);
		if (m_error.has_value())
		{
			return;
		}
	}

	for (std::size_t i = targets.size(); i > 0; --i)
	{
		EmitAssignmentTarget(*targets[i - 1]);
		if (m_error.has_value())
		{
			return;
		}
		CurrentEmitter().EmitOpcode(OP_POP);
	}
}

void CodegenVisitor::Visit(const IncrementASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	expr.GetTarget().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	CurrentEmitter().EmitConstant(Value(static_cast<RuntimeInt>(1)));
	CurrentEmitter().EmitOpcode(OP_ADD);
	EmitAssignmentTarget(expr.GetTarget());
	if (m_error.has_value())
	{
		return;
	}
	CurrentEmitter().EmitOpcode(OP_POP);
}

void CodegenVisitor::Visit(const ShortVariableDeclarationASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	const std::vector<std::string>& names = expr.GetNames();
	const std::vector<ASTNodePtr>& values = expr.GetValues();

	for (std::size_t i = 0; i < names.size(); ++i)
	{
		values[i]->Accept(*this);
		if (m_error.has_value())
		{
			return;
		}

		m_functionStack.back().DeclareLocal(names[i]);
	}
}

void CodegenVisitor::Visit(const VariableDeclarationASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	const std::vector<std::string>& names = expr.GetNames();
	const std::vector<ASTNodePtr>& values = expr.GetValues();

	if (values.empty())
	{
		const TypeDescriptor defaultType = expr.GetDeclaredType().value_or(Type::ERROR);
		for (const std::string& name : names)
		{
			EmitDefault(defaultType);
			m_functionStack.back().DeclareLocal(name);
		}
		return;
	}

	for (std::size_t i = 0; i < names.size(); ++i)
	{
		values[i]->Accept(*this);
		if (m_error.has_value())
		{
			return;
		}

		m_functionStack.back().DeclareLocal(names[i]);
	}
}

void CodegenVisitor::Visit(const ExpressionStatementASTNode& expr)
{
	expr.GetExpression().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	CurrentEmitter().EmitOpcode(OP_POP);
}

void CodegenVisitor::Visit(const ProgramASTNode& expr)
{
	expr.GetStatements().Accept(*this);
}

void CodegenVisitor::Visit(const StatementListASTNode& expr)
{
	for (const ASTNodePtr& statement : expr.GetStatements())
	{
		statement->Accept(*this);
		if (m_error.has_value())
		{
			return;
		}
	}
}

void CodegenVisitor::Visit(const BlockASTNode& expr)
{
	m_functionStack.back().BeginScope();
	expr.GetStatements().Accept(*this);
	const int localCount = m_functionStack.back().EndScope();
	if (m_error.has_value())
	{
		return;
	}

	for (int i = 0; i < localCount; ++i)
	{
		CurrentEmitter().EmitOpcode(OP_POP);
	}
}

void CodegenVisitor::Visit(const IfASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	expr.GetCondition().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	const int elseJump = CurrentEmitter().EmitJump(OP_JUMP_IF_FALSE);
	CurrentEmitter().EmitOpcode(OP_POP);
	expr.GetThenBranch().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	const int endJump = CurrentEmitter().EmitJump(OP_JUMP);
	CurrentEmitter().PatchJump(elseJump);
	CurrentEmitter().EmitOpcode(OP_POP);

	if (const ASTNode* elseBranch = expr.GetElseBranch())
	{
		elseBranch->Accept(*this);
		if (m_error.has_value())
		{
			return;
		}
	}

	CurrentEmitter().PatchJump(endJump);
}

void CodegenVisitor::Visit(const ForASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	const bool isClassicFor = expr.GetInitializer() != nullptr || expr.GetPost() != nullptr;
	const int outerScopeDepth = m_functionStack.back().CurrentScopeDepth();
	if (isClassicFor)
	{
		m_functionStack.back().BeginScope();
	}

	if (const ASTNode* initializer = expr.GetInitializer())
	{
		initializer->Accept(*this);
		if (m_error.has_value())
		{
			return;
		}
	}

	const int loopStart = CurrentEmitter().CurrentOffset();
	expr.GetCondition().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	const int exitJump = CurrentEmitter().EmitJump(OP_JUMP_IF_FALSE);
	CurrentEmitter().EmitOpcode(OP_POP);

	LoopContext loopContext;
	loopContext.breakScopeDepth = outerScopeDepth;
	loopContext.continueScopeDepth = isClassicFor
		? m_functionStack.back().CurrentScopeDepth()
		: outerScopeDepth;
	loopContext.continueTarget = loopStart;
	loopContext.continueJumpsForward = isClassicFor;
	m_loopStack.push_back(std::move(loopContext));

	expr.GetBody().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	LoopContext completedLoop = std::move(m_loopStack.back());
	m_loopStack.pop_back();

	if (const ASTNode* post = expr.GetPost())
	{
		const int postStart = CurrentEmitter().CurrentOffset();
		for (const int continueJump : completedLoop.continueJumps)
		{
			CurrentEmitter().PatchJump(continueJump);
		}
		post->Accept(*this);
		if (m_error.has_value())
		{
			return;
		}
		CurrentEmitter().EmitLoop(loopStart);
		(void)postStart;
	}
	else
	{
		CurrentEmitter().EmitLoop(loopStart);
	}

	CurrentEmitter().PatchJump(exitJump);
	CurrentEmitter().EmitOpcode(OP_POP);

	if (isClassicFor)
	{
		const int localCount = m_functionStack.back().EndScope();
		for (int i = 0; i < localCount; ++i)
		{
			CurrentEmitter().EmitOpcode(OP_POP);
		}
	}

	for (const int breakJump : completedLoop.breakJumps)
	{
		CurrentEmitter().PatchJump(breakJump);
	}
}

void CodegenVisitor::Visit(const BreakASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}
	if (m_loopStack.empty())
	{
		Fail("Break statement is not allowed outside loop.");
		return;
	}

	LoopContext& loopContext = m_loopStack.back();
	EmitScopeCleanup(loopContext.breakScopeDepth);
	loopContext.breakJumps.push_back(CurrentEmitter().EmitJump(OP_JUMP));
}

void CodegenVisitor::Visit(const ContinueASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}
	if (m_loopStack.empty())
	{
		Fail("Continue statement is not allowed outside loop.");
		return;
	}

	LoopContext& loopContext = m_loopStack.back();
	EmitScopeCleanup(loopContext.continueScopeDepth);
	if (loopContext.continueJumpsForward)
	{
		loopContext.continueJumps.push_back(CurrentEmitter().EmitJump(OP_JUMP));
		return;
	}

	CurrentEmitter().EmitLoop(loopContext.continueTarget);
}

void CodegenVisitor::Visit(const ReturnASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	if (const ASTNode* value = expr.GetValue())
	{
		value->Accept(*this);
		if (m_error.has_value())
		{
			return;
		}
	}
	else
	{
		CurrentEmitter().EmitConstant(Value());
	}

	CurrentEmitter().EmitOpcode(OP_RETURN);
}

void CodegenVisitor::Visit(const FunctionDeclarationASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	auto function = std::make_shared<ObjFunction>();
	function->name = std::make_shared<ObjString>(expr.GetQualifiedName());
	function->arity = static_cast<int>(expr.GetParameters().size() + (expr.GetReceiver() ? 1 : 0));

	m_functionStack.emplace_back(function, m_error);
	int parameterSlot = 1;
	if (const FunctionParameter* receiver = expr.GetReceiver())
	{
		m_functionStack.back().RegisterParameter(receiver->name, parameterSlot);
		++parameterSlot;
	}
	for (const FunctionParameter& parameter : expr.GetParameters())
	{
		m_functionStack.back().RegisterParameter(parameter.name, parameterSlot);
		++parameterSlot;
	}

	expr.GetBody().Accept(*this);
	if (!m_error.has_value())
	{
		CurrentEmitter().EmitConstant(Value());
		CurrentEmitter().EmitOpcode(OP_RETURN);
	}
	m_functionStack.pop_back();

	if (m_error.has_value())
	{
		return;
	}

	m_programContext.AddFunction(std::move(function));
}

void CodegenVisitor::Visit(const StructDeclarationASTNode&)
{
}

BytecodeEmitter& CodegenVisitor::CurrentEmitter()
{
	return m_functionStack.back().Emitter();
}

void CodegenVisitor::Fail(std::string message)
{
	if (!m_error.has_value())
	{
		m_error = std::move(message);
	}
}

void CodegenVisitor::EmitDefault(const TypeDescriptor& type)
{
	if (type.IsArray())
	{
		const int length = type.GetArrayLength();
		for (int index = 0; index < length; ++index)
		{
			EmitDefault(type.GetElementType());
			if (m_error.has_value())
			{
				return;
			}
		}
		CurrentEmitter().EmitArray(length);
		return;
	}
	if (type.IsNamed())
	{
		EmitStructDefault(type);
		return;
	}

	EmitScalarDefault(type.GetScalarType());
}

void CodegenVisitor::EmitStructDefault(const TypeDescriptor& type)
{
	const SemanticSymbol* symbol = m_symbols.Resolve(type.GetName());
	if (!symbol || symbol->kind != SemanticSymbolKind::TYPE)
	{
		Fail("Unknown struct type during code generation: " + type.GetName());
		return;
	}

	std::vector<std::string> fieldNames;
	fieldNames.reserve(symbol->fields.size());
	for (const FieldSignature& field : symbol->fields)
	{
		EmitDefault(field.type);
		if (m_error.has_value())
		{
			return;
		}
		fieldNames.push_back(field.name);
	}

	CurrentEmitter().EmitStruct(type.GetName(), fieldNames);
}

void CodegenVisitor::EmitScalarDefault(const Type type)
{
	switch (type)
	{
	case Type::INT:
		CurrentEmitter().EmitConstant(Value(static_cast<RuntimeInt>(0)));
		return;
	case Type::FLOAT:
		CurrentEmitter().EmitConstant(Value(0.0));
		return;
	case Type::BOOL:
		CurrentEmitter().EmitConstant(Value(false));
		return;
	case Type::STRING:
		CurrentEmitter().EmitConstant(Value(std::make_shared<ObjString>("")));
		return;
	default:
		CurrentEmitter().EmitConstant(Value());
	}
}

void CodegenVisitor::EmitBinaryOperation(const BinaryOperator op)
{
	if (op == BinaryOperator::OR || op == BinaryOperator::AND)
	{
		Fail("Logical operator should be emitted via short-circuit code path.");
		return;
	}

	const std::optional<OpCode> opcode = OperatorOpcodeResolver::Resolve(op);
	if (!opcode.has_value())
	{
		Fail("Unsupported binary operator during code generation.");
		return;
	}

	CurrentEmitter().EmitOpcode(*opcode);
}

void CodegenVisitor::EmitLogicalAnd(const BinaryASTNode& expr)
{
	expr.GetLeft().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	const int falseJump = CurrentEmitter().EmitJump(OP_JUMP_IF_FALSE);
	CurrentEmitter().EmitOpcode(OP_POP);
	expr.GetRight().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	CurrentEmitter().PatchJump(falseJump);
}

void CodegenVisitor::EmitLogicalOr(const BinaryASTNode& expr)
{
	expr.GetLeft().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	const int falseJump = CurrentEmitter().EmitJump(OP_JUMP_IF_FALSE);
	const int endJump = CurrentEmitter().EmitJump(OP_JUMP);
	CurrentEmitter().PatchJump(falseJump);
	CurrentEmitter().EmitOpcode(OP_POP);
	expr.GetRight().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	CurrentEmitter().PatchJump(endJump);
}

void CodegenVisitor::EmitScopeCleanup(const int scopeDepth)
{
	const int localCount = m_functionStack.back().CountLocalsAboveDepth(scopeDepth);
	for (int i = 0; i < localCount; ++i)
	{
		CurrentEmitter().EmitOpcode(OP_POP);
	}
}

void CodegenVisitor::EmitAssignmentTarget(const ASTNode& target)
{
	if (const auto* identifier = dynamic_cast<const IdentifierASTNode*>(&target))
	{
		const std::string& name = identifier->GetName();
		if (const std::optional<int> localSlot = m_functionStack.back().ResolveLocal(name))
		{
			CurrentEmitter().EmitLocalSet(*localSlot);
		}
		else
		{
			CurrentEmitter().EmitGlobalSet(name);
		}
		return;
	}

	if (const auto* index = dynamic_cast<const IndexASTNode*>(&target))
	{
		index->GetObject().Accept(*this);
		if (m_error.has_value())
		{
			return;
		}
		index->GetIndex().Accept(*this);
		if (m_error.has_value())
		{
			return;
		}
		CurrentEmitter().EmitIndexSet();
		return;
	}

	if (const auto* member = dynamic_cast<const MemberAccessASTNode*>(&target))
	{
		member->GetObject().Accept(*this);
		if (m_error.has_value())
		{
			return;
		}
		CurrentEmitter().EmitMemberSet(member->GetMember());
		return;
	}

	Fail("Unsupported assignment target during code generation.");
}

bool CodegenVisitor::EnsureTyped(const ASTNode& expr)
{
	if (expr.GetInferredType().has_value())
	{
		return true;
	}

	Fail("Typed AST information is missing for code generation.");
	return false;
}

void CodegenVisitor::EmitIdentifierRef(const IdentifierASTNode& expr)
{
	if (const std::optional<int> localSlot = m_functionStack.back().ResolveLocal(expr.GetName()))
	{
		CurrentEmitter().EmitLocalRef(*localSlot);
		return;
	}

	CurrentEmitter().EmitGlobalRef(expr.GetName());
}

const MethodSignature* CodegenVisitor::ResolveMethod(const TypeDescriptor& receiverType, const std::string& methodName) const
{
	if (!receiverType.IsNamed())
	{
		return nullptr;
	}

	const SemanticSymbol* typeSymbol = m_symbols.Resolve(receiverType.GetName());
	if (!typeSymbol || typeSymbol->kind != SemanticSymbolKind::TYPE)
	{
		return nullptr;
	}

	for (const MethodSignature& method : typeSymbol->methods)
	{
		if (method.name == methodName)
		{
			return &method;
		}
	}

	return nullptr;
}

bool CodegenVisitor::ShouldPassArgumentByPointer(const std::string& calleeName, const std::size_t argumentIndex) const
{
	if (calleeName == "scan")
	{
		return argumentIndex == 0;
	}

	const SemanticSymbol* symbol = m_symbols.Resolve(calleeName);
	if (!symbol || argumentIndex >= symbol->parameters.size())
	{
		return false;
	}

	return symbol->parameters[argumentIndex].isPointer;
}
