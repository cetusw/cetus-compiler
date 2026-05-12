#include "CodegenVisitor.h"

#include "src/backend/vm/objects/ObjFunction.h"
#include "src/backend/vm/objects/ObjString.h"
#include "src/backend/vm/types/Chunk.h"
#include "src/frontend/syntax/ast/Expr.h"
#include <limits>

CodegenVisitor::CodegenVisitor(const SymbolTable& symbols, const TypeCheckResult& typeInfo)
	: m_symbols(symbols)
	, m_typeInfo(typeInfo)
{
}

CodegenResult CodegenVisitor::Generate(const Expr& expr)
{
	m_function = std::make_shared<ObjFunction>();
	m_function->name = std::make_shared<ObjString>("expr");
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

	EmitOpcode(OP_RETURN);
	return CodegenResult::Success(m_function);
}

void CodegenVisitor::Visit(const BoolLiteralExpr& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	EmitConstant(Value(expr.GetValue()));
}

void CodegenVisitor::Visit(const IntLiteralExpr& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	EmitConstant(Value(std::stod(expr.GetValue())));
}

void CodegenVisitor::Visit(const FloatLiteralExpr& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}

	EmitConstant(Value(std::stod(expr.GetValue())));
}

void CodegenVisitor::Visit(const IdentifierExpr& expr)
{
	if (!EnsureTyped(expr))
	{
		return;
	}
	if (!m_symbols.Resolve(expr.GetName()))
	{
		Fail("Undefined identifier during code generation: " + expr.GetName());
		return;
	}

	EmitGlobalLoad(expr.GetName());
}

void CodegenVisitor::Visit(const UnaryExpr& expr)
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

	switch (expr.GetOperator())
	{
	case UnaryOperator::NEGATE:
		EmitOpcode(OP_NEGATE);
		return;
	case UnaryOperator::NOT:
		EmitOpcode(OP_NOT);
		return;
	}

	Fail("Unsupported unary operator during code generation.");
}

void CodegenVisitor::Visit(const BinaryExpr& expr)
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

void CodegenVisitor::Visit(const MemberAccessExpr&)
{
	Fail("Member access code generation is not implemented yet.");
}

void CodegenVisitor::Visit(const IndexExpr&)
{
	Fail("Index access code generation is not implemented yet.");
}

void CodegenVisitor::Visit(const AssignmentExpr&)
{
	Fail("Assignment code generation is not implemented for VM bytecode yet.");
}

void CodegenVisitor::Visit(const SequenceExpr&)
{
	Fail("Sequence code generation is not implemented for VM bytecode yet.");
}

Chunk& CodegenVisitor::CurrentChunk() const
{
	return m_function->chunk;
}

void CodegenVisitor::Fail(std::string message)
{
	if (!m_error.has_value())
	{
		m_error = std::move(message);
	}
}

void CodegenVisitor::EmitByte(const uint8_t byte) const
{
	CurrentChunk().Write(byte, DEFAULT_LINE);
}

void CodegenVisitor::EmitOpcode(const OpCode opcode) const
{
	EmitByte(opcode);
}

void CodegenVisitor::EmitOperandByte(const int value)
{
	if (value < 0 || value > std::numeric_limits<uint8_t>::max())
	{
		Fail("Bytecode operand exceeds 1-byte limit.");
		return;
	}

	EmitByte(static_cast<uint8_t>(value));
}

void CodegenVisitor::EmitConstant(const Value& value)
{
	const int constantIndex = CurrentChunk().AddConstant(value);
	EmitOpcode(OP_CONSTANT);
	EmitOperandByte(constantIndex);
}

int CodegenVisitor::EmitJump(const OpCode opcode) const
{
	EmitOpcode(opcode);
	EmitByte(0xff);
	EmitByte(0xff);
	return CurrentChunk().GetCodeSize() - 2;
}

void CodegenVisitor::PatchJump(const int jumpOffset)
{
	const int jumpDistance = CurrentChunk().GetCodeSize() - jumpOffset - 2;
	if (jumpDistance < 0 || jumpDistance > std::numeric_limits<uint16_t>::max())
	{
		Fail("Jump offset exceeds 2-byte limit.");
		return;
	}

	EmitShortOperand(jumpDistance, jumpOffset);
}

void CodegenVisitor::EmitShortOperand(const int value, const int patchOffset) const
{
	CurrentChunk().PatchByte(patchOffset, static_cast<uint8_t>(value >> 8 & 0xff));
	CurrentChunk().PatchByte(patchOffset + 1, static_cast<uint8_t>(value & 0xff));
}

void CodegenVisitor::EmitBinaryOperation(const BinaryOperator op)
{
	switch (op)
	{
	case BinaryOperator::ADD: EmitOpcode(OP_ADD); return;
	case BinaryOperator::SUBTRACT: EmitOpcode(OP_SUBTRACT); return;
	case BinaryOperator::MULTIPLY: EmitOpcode(OP_MULTIPLY); return;
	case BinaryOperator::DIVIDE: EmitOpcode(OP_DIVIDE); return;
	case BinaryOperator::MODULO: EmitOpcode(OP_MODULO); return;
	case BinaryOperator::LESS: EmitOpcode(OP_LESS); return;
	case BinaryOperator::LESS_EQUAL: EmitOpcode(OP_LESS_OR_EQUAL); return;
	case BinaryOperator::NOT_EQUAL: EmitOpcode(OP_NOT_EQUAL); return;
	case BinaryOperator::EQUAL: EmitOpcode(OP_EQUAL); return;
	case BinaryOperator::GREATER: EmitOpcode(OP_GREATER); return;
	case BinaryOperator::GREATER_EQUAL: EmitOpcode(OP_GREATER_OR_EQUAL); return;
	case BinaryOperator::OR:
	case BinaryOperator::AND:
		Fail("Logical operator should be emitted via short-circuit code path.");
		return;
	}

	Fail("Unsupported binary operator during code generation.");
}

void CodegenVisitor::EmitLogicalAnd(const BinaryExpr& expr)
{
	expr.GetLeft().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	const int falseJump = EmitJump(OP_JUMP_IF_FALSE);
	EmitOpcode(OP_POP);
	expr.GetRight().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	PatchJump(falseJump);
}

void CodegenVisitor::EmitLogicalOr(const BinaryExpr& expr)
{
	expr.GetLeft().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	const int falseJump = EmitJump(OP_JUMP_IF_FALSE);
	const int endJump = EmitJump(OP_JUMP);
	PatchJump(falseJump);
	EmitOpcode(OP_POP);
	expr.GetRight().Accept(*this);
	if (m_error.has_value())
	{
		return;
	}

	PatchJump(endJump);
}

void CodegenVisitor::EmitGlobalLoad(const std::string& name)
{
	EmitOpcode(OP_GET_GLOBAL);
	const int constantIndex = CurrentChunk().AddConstant(Value(std::make_shared<ObjString>(name)));
	EmitOperandByte(constantIndex);
}

bool CodegenVisitor::EnsureTyped(const Expr& expr)
{
	if (m_typeInfo.GetNodeType(expr).has_value())
	{
		return true;
	}

	Fail("Typed AST information is missing for code generation.");
	return false;
}
