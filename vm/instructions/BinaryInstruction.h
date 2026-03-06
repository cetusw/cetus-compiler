#pragma once
#include "../vm.h"
#include "Instruction.h"
#include "types/value.h"

class VM;

template <typename, bool = true>
class BinaryInstruction final : public Instruction
{
public:
	InterpretResult Execute(VM& vm) const override;
};

struct AddOp
{
	Value operator()(const Value& a, const Value& b) const { return a + b; }
};
struct SubOp
{
	Value operator()(const Value& a, const Value& b) const { return a - b; }
};
struct MulOp
{
	Value operator()(const Value& a, const Value& b) const { return a * b; }
};
struct DivOp
{
	Value operator()(const Value& a, const Value& b) const { return a / b; }
};
struct EqualOp
{
	Value operator()(const Value& a, const Value& b) const { return a == b; }
};
struct GreaterOp
{
	Value operator()(const Value& a, const Value& b) const { return Value(a > b); }
};
struct LessOp
{
	Value operator()(const Value& a, const Value& b) const { return Value(a < b); }
};
struct GreaterOrEqualOp
{
	Value operator()(const Value& a, const Value& b) const { return Value(a >= b); }
};
struct LessOrEqualOp
{
	Value operator()(const Value& a, const Value& b) const { return Value(a <= b); }
};

template <typename Op, bool MustBeNumber>
InterpretResult BinaryInstruction<Op, MustBeNumber>::Execute(VM& vm) const
{
	if (MustBeNumber && (!vm.Peek(0).IsNumber() || !vm.Peek(1).IsNumber()))
	{
		std::fprintf(stderr, "Operands must be numbers.\n");
		return InterpretResult::RUNTIME_ERROR;
	}
	Value b = vm.Pop();
	Value a = vm.Pop();
	vm.Push(Op()(a, b));
	return InterpretResult::OK;
}