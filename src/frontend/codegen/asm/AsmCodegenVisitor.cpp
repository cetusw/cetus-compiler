#include "AsmCodegenVisitor.h"

#include "src/frontend/semantic/Type.h"
#include "src/frontend/syntax/ast/ASTNode.h"

#include <unordered_map>
#include <utility>

namespace
{
const std::unordered_map<BinaryOperator, std::string>& SimpleArithmeticInstructions()
{
	static const std::unordered_map<BinaryOperator, std::string> instructions = {
		{ BinaryOperator::ADD, "add" },
		{ BinaryOperator::SUBTRACT, "sub" },
		{ BinaryOperator::MULTIPLY, "imul" },
	};
	return instructions;
}

const std::unordered_map<BinaryOperator, std::string>& ComparisonInstructions()
{
	static const std::unordered_map<BinaryOperator, std::string> instructions = {
		{ BinaryOperator::LESS, "setl" },
		{ BinaryOperator::LESS_EQUAL, "setle" },
		{ BinaryOperator::NOT_EQUAL, "setne" },
		{ BinaryOperator::EQUAL, "sete" },
		{ BinaryOperator::GREATER, "setg" },
		{ BinaryOperator::GREATER_EQUAL, "setge" },
	};
	return instructions;
}
} // namespace

AsmCodegenVisitor::AsmCodegenVisitor(const TypeCheckResult& typeInfo)
	: m_typeInfo(typeInfo)
{
}

AsmCodegenResult AsmCodegenVisitor::Generate(const ASTNode& node)
{
	m_program = AsmProgram();
	m_declaredVariables.clear();
	m_error.reset();
	m_nextLabelId = 0;

	if (!EnsureSupportedType(node))
	{
		return AsmCodegenResult::Error(*m_error);
	}

	EmitProgramPreamble();
	node.Accept(*this);
	if (m_error.has_value())
	{
		return AsmCodegenResult::Error(*m_error);
	}

	EmitProgramPostamble();
	return AsmCodegenResult::Success(m_program.Build());
}

void AsmCodegenVisitor::Visit(const BoolLiteralASTNode& node)
{
	if (!EnsureSupportedType(node))
	{
		return;
	}

	Emit(std::string("    mov rax, ") + (node.GetValue() ? "1" : "0"));
}

void AsmCodegenVisitor::Visit(const IntLiteralASTNode& node)
{
	if (!EnsureSupportedType(node))
	{
		return;
	}

	Emit("    mov rax, " + node.GetValue());
}

void AsmCodegenVisitor::Visit(const FloatLiteralASTNode&)
{
	Fail("Float literals are not supported by asm code generation yet.");
}

void AsmCodegenVisitor::Visit(const IdentifierASTNode& node)
{
	if (!EnsureSupportedType(node))
	{
		return;
	}

	EmitLoadVariable(node.GetName());
}

void AsmCodegenVisitor::Visit(const UnaryASTNode& node)
{
	if (!EnsureSupportedType(node))
	{
		return;
	}

	node.GetOperand().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	node.GetOperator() == UnaryOperator::NEGATE
		? EmitUnaryNegation(node)
		: EmitLogicalNot(node);
}

void AsmCodegenVisitor::Visit(const BinaryASTNode& node)
{
	if (!EnsureSupportedType(node))
	{
		return;
	}
	if (!IsSupportedBinary(node.GetOperator()))
	{
		Fail("Unsupported binary operator during asm code generation.");
		return;
	}

	if (EmitShortCircuitIfNeeded(node))
	{
		return;
	}

	EmitBinaryOperands(node);
	if (m_error.has_value())
	{
		return;
	}
	EmitBinaryOperation(node.GetOperator());
}

void AsmCodegenVisitor::Visit(const MemberAccessASTNode&)
{
	Fail("Member access is not supported by x86-64 asm code generation yet.");
}

void AsmCodegenVisitor::Visit(const IndexASTNode&)
{
	Fail("Index access is not supported by x86-64 asm code generation yet.");
}

void AsmCodegenVisitor::Visit(const AssignmentASTNode& node)
{
	if (!EnsureSupportedType(node))
	{
		return;
	}

	node.GetValue().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	EmitStoreVariable(node.GetName());
}

void AsmCodegenVisitor::Visit(const SequenceASTNode& node)
{
	if (!EnsureSupportedType(node))
	{
		return;
	}

	for (const ASTNodePtr& child : node.GetExpressions())
	{
		child->Accept(*this);
		if (m_error.has_value())
		{
			return;
		}
	}
}

void AsmCodegenVisitor::Visit(const IfElseASTNode& node)
{
	if (!EnsureSupportedType(node))
	{
		return;
	}

	const std::string elseLabel = NextLabel(".L_if_else");
	const std::string endLabel = NextLabel(".L_if_end");

	node.GetCondition().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	Emit("    cmp rax, 0");
	Emit("    je " + elseLabel);

	node.GetThenBranch().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	Emit("    jmp " + endLabel);

	m_program.AddText(elseLabel + ":");
	node.GetElseBranch().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	m_program.AddText(endLabel + ":");
}

void AsmCodegenVisitor::Visit(const PrintfASTNode& node)
{
	if (!EnsureSupportedType(node))
	{
		return;
	}

	node.GetArgument().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	EmitPrintf();
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
	m_program.AddRoData(R"(    .string "%lld\n")");

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

	Emit("    ");
	Emit("    // postamble");
	Emit("    mov eax, 0");
	Emit("    leave");
	Emit("    ret");
}

void AsmCodegenVisitor::EmitLoadVariable(const std::string& name)
{
	EnsureVariableDeclared(name);
	Emit("    mov rax, qword ptr [rip + " + name + "]");
}

void AsmCodegenVisitor::EmitStoreVariable(const std::string& name)
{
	EnsureVariableDeclared(name);
	Emit("    mov qword ptr [rip + " + name + "], rax");
}

void AsmCodegenVisitor::EmitUnaryNegation(const UnaryASTNode& node)
{
	if (!IsIntTyped(node.GetOperand()))
	{
		Fail("Unary '-' expects int operand in asm code generation.");
		return;
	}
	Emit("    neg rax");
}

void AsmCodegenVisitor::EmitLogicalNot(const UnaryASTNode& node)
{
	if (!IsFalsey(node.GetOperand()))
	{
		Fail("Unary '!' expects truthy-compatible operand in asm code generation.");
		return;
	}
	Emit("    cmp rax, 0");
	Emit("    sete al");
	Emit("    movzx rax, al");
}

bool AsmCodegenVisitor::EmitShortCircuitIfNeeded(const BinaryASTNode& node)
{
	if (node.GetOperator() == BinaryOperator::AND)
	{
		EmitLogicalAnd(node);
		return true;
	}
	if (node.GetOperator() == BinaryOperator::OR)
	{
		EmitLogicalOr(node);
		return true;
	}
	return false;
}

void AsmCodegenVisitor::EmitBinaryOperands(const BinaryASTNode& node)
{
	node.GetLeft().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	Emit("    push rax");

	node.GetRight().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	Emit("    pop rbx");
}

void AsmCodegenVisitor::EmitBinaryOperation(const BinaryOperator op)
{
	if (IsSimpleArithmetic(op))
	{
		EmitSimpleArithmetic(op);
		return;
	}
	if (op == BinaryOperator::DIVIDE)
	{
		EmitDivision();
		return;
	}
	if (op == BinaryOperator::MODULO)
	{
		EmitModulo();
		return;
	}
	if (IsComparison(op))
	{
		EmitBinaryComparison(op);
		return;
	}
	Fail("Unsupported binary operator during asm code generation.");
}

void AsmCodegenVisitor::EmitSimpleArithmetic(const BinaryOperator op)
{
	Emit("    " + SimpleArithmeticInstructions().at(op) + " rbx, rax");
	Emit("    mov rax, rbx");
}

void AsmCodegenVisitor::EmitDivision()
{
	Emit("    mov rcx, rax");
	Emit("    mov rax, rbx");
	Emit("    cqo");
	Emit("    idiv rcx");
}

void AsmCodegenVisitor::EmitModulo()
{
	EmitDivision();
	Emit("    mov rax, rdx");
}

void AsmCodegenVisitor::EmitBinaryComparison(const BinaryOperator op)
{
	Emit("    cmp rbx, rax");
	Emit("    " + ComparisonInstructions().at(op) + " al");
	Emit("    movzx rax, al");
}

void AsmCodegenVisitor::EmitLogicalAnd(const BinaryASTNode& node)
{
	const std::string falseLabel = NextLabel(".L_and_false");
	const std::string endLabel = NextLabel(".L_and_end");

	node.GetLeft().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	Emit("    cmp rax, 0");
	Emit("    je " + falseLabel);

	node.GetRight().Accept(*this);
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

void AsmCodegenVisitor::EmitLogicalOr(const BinaryASTNode& node)
{
	const std::string trueLabel = NextLabel(".L_or_true");
	const std::string endLabel = NextLabel(".L_or_end");

	node.GetLeft().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}
	Emit("    cmp rax, 0");
	Emit("    jne " + trueLabel);

	node.GetRight().Accept(*this);
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

void AsmCodegenVisitor::EmitPrintf()
{
	Emit("    mov rsi, rax");
	Emit("    lea rdi, [rip + fmt_int]");
	Emit("    xor eax, eax");
	Emit("    call printf");
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

bool AsmCodegenVisitor::EnsureTyped(const ASTNode& node)
{
	if (node.GetInferredType().has_value())
	{
		return true;
	}

	Fail("Typed AST information is missing for asm code generation.");
	return false;
}

bool AsmCodegenVisitor::EnsureSupportedType(const ASTNode& node)
{
	if (!EnsureTyped(node))
	{
		return false;
	}

	if (IsIntTyped(node) || IsBoolTyped(node))
	{
		return true;
	}

	Fail("Only int and bool expressions are supported by x86-64 asm code generation.");
	return false;
}

bool AsmCodegenVisitor::IsIntTyped(const ASTNode& node)
{
	return node.GetInferredType() == Type::INT;
}

bool AsmCodegenVisitor::IsBoolTyped(const ASTNode& node)
{
	return node.GetInferredType() == Type::BOOL;
}

bool AsmCodegenVisitor::IsFalsey(const ASTNode& node)
{
	return IsIntTyped(node) || IsBoolTyped(node);
}

bool AsmCodegenVisitor::IsSupportedBinary(const BinaryOperator op)
{
	return op == BinaryOperator::OR
		|| op == BinaryOperator::AND
		|| op == BinaryOperator::DIVIDE
		|| op == BinaryOperator::MODULO
		|| IsSimpleArithmetic(op)
		|| IsComparison(op);
}

bool AsmCodegenVisitor::IsSimpleArithmetic(const BinaryOperator op)
{
	return SimpleArithmeticInstructions().contains(op);
}

bool AsmCodegenVisitor::IsComparison(const BinaryOperator op)
{
	return ComparisonInstructions().contains(op);
}
