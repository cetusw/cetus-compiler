#include "AsmCodegenVisitor.h"
#include "src/frontend/syntax/ast/Expr.h"
#include "src/frontend/syntax/semantic/Type.h"
#include <utility>

AsmCodegenVisitor::AsmCodegenVisitor(const TypeCheckResult& typeInfo)
	: m_typeInfo(typeInfo)
{
}

AsmCodegenResult AsmCodegenVisitor::Generate(const Expr& expr)
{
	m_program = AsmProgram();
	m_declaredVariables.clear();
	m_error.reset();
	m_nextLabelId = 0;

	if (!EnsureSupportedType(expr))
	{
		return AsmCodegenResult::Error(*m_error);
	}

	EmitProgramPreamble();
	expr.Accept(*this);
	if (m_error.has_value())
	{
		return AsmCodegenResult::Error(*m_error);
	}

	EmitProgramPostamble();
	return AsmCodegenResult::Success(m_program.Build());
}

void AsmCodegenVisitor::Visit(const BoolLiteralExpr& expr)
{
	if (!EnsureSupportedType(expr))
	{
		return;
	}

	Emit(std::string("    mov rax, ") + (expr.GetValue() ? "1" : "0"));
}

void AsmCodegenVisitor::Visit(const IntLiteralExpr& expr)
{
	if (!EnsureSupportedType(expr))
	{
		return;
	}

	Emit("    mov rax, " + expr.GetValue());
}

void AsmCodegenVisitor::Visit(const FloatLiteralExpr&)
{
	Fail("Float literals are not supported by x86-64 asm code generation yet.");
}

void AsmCodegenVisitor::Visit(const IdentifierExpr& expr)
{
	if (!EnsureSupportedType(expr))
	{
		return;
	}

	EnsureVariableDeclared(expr.GetName());
	Emit("    mov rax, qword ptr [rip + " + expr.GetName() + "]");
}

void AsmCodegenVisitor::Visit(const UnaryExpr& expr)
{
	if (!EnsureSupportedType(expr))
	{
		return;
	}

	expr.GetOperand().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	switch (expr.GetOperator())
	{
	case UnaryOperator::NEGATE:
		if (!IsIntTyped(expr.GetOperand()))
		{
			Fail("Unary '-' expects int operand in x86-64 asm code generation.");
			return;
		}
		Emit("    neg rax");
		return;
	case UnaryOperator::NOT:
		if (!IsBoolTyped(expr.GetOperand()))
		{
			Fail("Unary '!' expects bool operand in x86-64 asm code generation.");
			return;
		}
		Emit("    cmp rax, 0");
		Emit("    sete al");
		Emit("    movzx rax, al");
		return;
	}

	Fail("Unsupported unary operator during asm code generation.");
}

void AsmCodegenVisitor::Visit(const BinaryExpr& expr)
{
	if (!EnsureSupportedType(expr))
	{
		return;
	}
	if (!IsSupportedBinary(expr.GetOperator()))
	{
		Fail("Unsupported binary operator during asm code generation.");
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

	Emit("    push rax");

	expr.GetRight().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	Emit("    pop rbx");

	switch (expr.GetOperator())
	{
	case BinaryOperator::ADD:
		Emit("    add rbx, rax");
		Emit("    mov rax, rbx");
		break;
	case BinaryOperator::SUBTRACT:
		Emit("    sub rbx, rax");
		Emit("    mov rax, rbx");
		break;
	case BinaryOperator::MULTIPLY:
		Emit("    imul rbx, rax");
		Emit("    mov rax, rbx");
		break;
	case BinaryOperator::DIVIDE:
		Emit("    mov rcx, rax");
		Emit("    mov rax, rbx");
		Emit("    cqo");
		Emit("    idiv rcx");
		break;
	case BinaryOperator::MODULO:
		Emit("    mov rcx, rax");
		Emit("    mov rax, rbx");
		Emit("    cqo");
		Emit("    idiv rcx");
		Emit("    mov rax, rdx");
		break;
	case BinaryOperator::LESS:
	case BinaryOperator::LESS_EQUAL:
	case BinaryOperator::NOT_EQUAL:
	case BinaryOperator::EQUAL:
	case BinaryOperator::GREATER:
	case BinaryOperator::GREATER_EQUAL:
		EmitBinaryComparison(expr.GetOperator());
		break;
	case BinaryOperator::OR:
	case BinaryOperator::AND:
		Fail("Logical operator should be emitted via short-circuit code path.");
	}
}

void AsmCodegenVisitor::Visit(const MemberAccessExpr&)
{
	Fail("Member access is not supported by x86-64 asm code generation yet.");
}

void AsmCodegenVisitor::Visit(const IndexExpr&)
{
	Fail("Index access is not supported by x86-64 asm code generation yet.");
}

void AsmCodegenVisitor::Visit(const AssignmentExpr& expr)
{
	if (!EnsureSupportedType(expr))
	{
		return;
	}

	EnsureVariableDeclared(expr.GetName());
	expr.GetValue().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	Emit("    mov qword ptr [rip + " + expr.GetName() + "], rax");
}

void AsmCodegenVisitor::Visit(const SequenceExpr& expr)
{
	if (!EnsureSupportedType(expr))
	{
		return;
	}

	for (const ExprPtr& child : expr.GetExpressions())
	{
		child->Accept(*this);
		if (m_error.has_value())
		{
			return;
		}
	}
}

void AsmCodegenVisitor::Emit(std::string line)
{
	if (m_error.has_value())
	{
		return;
	}

	m_program.AddText(std::move(line));
}

void AsmCodegenVisitor::EmitProgramPreamble()
{
	m_program.AddRoData("fmt_int:");
	m_program.AddRoData("    .string \"%lld\\n\"");
	m_program.AddText(".globl main");
	m_program.AddText(".extern printf");
	m_program.AddText("main:");
	m_program.AddText("    push rbp");
	m_program.AddText("    mov rbp, rsp");
}

void AsmCodegenVisitor::EmitProgramPostamble()
{
	if (m_error.has_value())
	{
		return;
	}

	Emit("    mov rsi, rax");
	Emit("    lea rdi, [rip + fmt_int]");
	Emit("    xor eax, eax");
	Emit("    call printf");
	Emit("    mov eax, 0");
	Emit("    leave");
	Emit("    ret");
}

void AsmCodegenVisitor::EmitBinaryComparison(const BinaryOperator op)
{
	Emit("    cmp rbx, rax");
	switch (op)
	{
	case BinaryOperator::LESS:
		Emit("    setl al");
		break;
	case BinaryOperator::LESS_EQUAL:
		Emit("    setle al");
		break;
	case BinaryOperator::NOT_EQUAL:
		Emit("    setne al");
		break;
	case BinaryOperator::EQUAL:
		Emit("    sete al");
		break;
	case BinaryOperator::GREATER:
		Emit("    setg al");
		break;
	case BinaryOperator::GREATER_EQUAL:
		Emit("    setge al");
		break;
	case BinaryOperator::OR:
	case BinaryOperator::AND:
	case BinaryOperator::ADD:
	case BinaryOperator::SUBTRACT:
	case BinaryOperator::MULTIPLY:
	case BinaryOperator::DIVIDE:
	case BinaryOperator::MODULO:
		Fail("Unsupported comparison operator during asm code generation.");
		return;
	}
	Emit("    movzx rax, al");
}

void AsmCodegenVisitor::EmitLogicalAnd(const BinaryExpr& expr)
{
	const std::string falseLabel = NextLabel(".L_and_false");
	const std::string endLabel = NextLabel(".L_and_end");

	expr.GetLeft().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	Emit("    cmp rax, 0");
	Emit("    je " + falseLabel);

	expr.GetRight().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	Emit("    cmp rax, 0");
	Emit("    je " + falseLabel);
	Emit("    mov rax, 1");
	Emit("    jmp " + endLabel);
	m_program.AddText(falseLabel + ":");
	Emit("    mov rax, 0");
	m_program.AddText(endLabel + ":");
}

void AsmCodegenVisitor::EmitLogicalOr(const BinaryExpr& expr)
{
	const std::string trueLabel = NextLabel(".L_or_true");
	const std::string endLabel = NextLabel(".L_or_end");

	expr.GetLeft().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	Emit("    cmp rax, 0");
	Emit("    jne " + trueLabel);

	expr.GetRight().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	Emit("    cmp rax, 0");
	Emit("    jne " + trueLabel);
	Emit("    mov rax, 0");
	Emit("    jmp " + endLabel);
	m_program.AddText(trueLabel + ":");
	Emit("    mov rax, 1");
	m_program.AddText(endLabel + ":");
}

void AsmCodegenVisitor::EnsureVariableDeclared(const std::string& name)
{
	if (m_declaredVariables.insert(name).second)
	{
		m_program.AddData(name + ":");
		m_program.AddData("    .quad 0");
	}
}

std::string AsmCodegenVisitor::NextLabel(const std::string& prefix)
{
	return prefix + "_" + std::to_string(m_nextLabelId++);
}

void AsmCodegenVisitor::Fail(std::string message)
{
	if (!m_error.has_value())
	{
		m_error = std::move(message);
	}
}

bool AsmCodegenVisitor::EnsureTyped(const Expr& expr)
{
	if (m_typeInfo.GetNodeType(expr).has_value())
	{
		return true;
	}

	Fail("Typed AST information is missing for asm code generation.");
	return false;
}

bool AsmCodegenVisitor::EnsureSupportedType(const Expr& expr)
{
	if (!EnsureTyped(expr))
	{
		return false;
	}

	if (IsIntTyped(expr) || IsBoolTyped(expr))
	{
		return true;
	}

	Fail("Only int and bool expressions are supported by x86-64 asm code generation.");
	return false;
}

bool AsmCodegenVisitor::IsIntTyped(const Expr& expr) const
{
	return m_typeInfo.GetNodeType(expr) == Type::INT;
}

bool AsmCodegenVisitor::IsBoolTyped(const Expr& expr) const
{
	return m_typeInfo.GetNodeType(expr) == Type::BOOL;
}

bool AsmCodegenVisitor::IsSupportedBinary(const BinaryOperator op)
{
	switch (op)
	{
	case BinaryOperator::OR:
	case BinaryOperator::AND:
	case BinaryOperator::ADD:
	case BinaryOperator::SUBTRACT:
	case BinaryOperator::MULTIPLY:
	case BinaryOperator::DIVIDE:
	case BinaryOperator::MODULO:
	case BinaryOperator::LESS:
	case BinaryOperator::LESS_EQUAL:
	case BinaryOperator::NOT_EQUAL:
	case BinaryOperator::EQUAL:
	case BinaryOperator::GREATER:
	case BinaryOperator::GREATER_EQUAL:
		return true;
	}

	return false;
}
