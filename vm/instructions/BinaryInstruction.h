#pragma once
#include "../vm.h"
#include "Instruction.h"
#include "types/value.h"

class VM;

template <typename Op>
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

template <typename Op>
InterpretResult BinaryInstruction<Op>::Execute(VM& vm) const
{
	if (!vm.Peek(0).IsNumber() || !vm.Peek(1).IsNumber())
	{
		std::fprintf(stderr, "Operands must be numbers.\n");
		return InterpretResult::RUNTIME_ERROR;
	}
	Value b = vm.Pop();
	Value a = vm.Pop();
	vm.Push(Op()(a, b));
	return InterpretResult::OK;
}