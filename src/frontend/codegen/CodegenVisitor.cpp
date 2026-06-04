#include "CodegenVisitor.h"

#include "OperatorOpcodeResolver.h"
#include "src/backend/vm/objects/ObjFunction.h"
#include "src/backend/vm/objects/ObjString.h"
#include "src/frontend/syntax/ast/ASTNode.h"

namespace
{
constexpr auto PRINTF_NATIVE_NAME = "println";
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

// TODO в VM нужно поддержать тип int, кроме double, чтобы различать int и float на уровне бекенда
void CodegenVisitor::Visit(const IntLiteralASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	CurrentEmitter().EmitConstant(Value(std::stod(expr.GetValue())));
}

void CodegenVisitor::Visit(const FloatLiteralASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	CurrentEmitter().EmitConstant(Value(std::stod(expr.GetValue())));
}

void CodegenVisitor::Visit(const StringLiteralASTNode&)
{
	Fail("String literal code generation is not implemented for VM bytecode yet.");
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

void CodegenVisitor::Visit(const MemberAccessASTNode&)
{
	Fail("Member access code generation is not implemented yet.");
}

void CodegenVisitor::Visit(const IndexASTNode&)
{
	Fail("Index access code generation is not implemented yet.");
}

void CodegenVisitor::Visit(const CallExpressionASTNode& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	const std::string& calleeName = expr.GetCalleeName();
	const std::string targetName = calleeName == "printf" ? PRINTF_NATIVE_NAME : calleeName;
	CurrentEmitter().EmitGlobalLoad(targetName);

	for (const ASTNodePtr& argument : expr.GetArguments())
	{
		argument->Accept(*this);
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

	const std::vector<std::string>& names = expr.GetNames();
	const std::vector<ASTNodePtr>& values = expr.GetValues();

	for (std::size_t i = 0; i < names.size(); ++i)
	{
		values[i]->Accept(*this);
		if (m_error.has_value())
		{
			return;
		}

		CurrentEmitter().EmitGlobalSet(names[i]);
		CurrentEmitter().EmitOpcode(OP_POP);
	}
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

		CurrentEmitter().EmitGlobalDefine(names[i]);
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
		const Type defaultType = expr.GetDeclaredType().value_or(Type::ERROR);
		for (const std::string& name : names)
		{
			EmitDefault(defaultType);
			CurrentEmitter().EmitGlobalDefine(name);
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

		CurrentEmitter().EmitGlobalDefine(names[i]);
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
	expr.GetStatements().Accept(*this);
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
	function->name = std::make_shared<ObjString>(expr.GetName());
	function->arity = static_cast<int>(expr.GetParameters().size());

	m_functionStack.emplace_back(function, m_error);
	int parameterSlot = 1;
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

void CodegenVisitor::EmitDefault(const Type type)
{
	switch (type)
	{
	case Type::INT:
	case Type::FLOAT:
		CurrentEmitter().EmitConstant(Value(0.0));
		return;
	case Type::BOOL:
		CurrentEmitter().EmitConstant(Value(false));
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

bool CodegenVisitor::EnsureTyped(const ASTNode& expr)
{
	if (expr.GetInferredType().has_value())
	{
		return true;
	}

	Fail("Typed AST information is missing for code generation.");
	return false;
}
