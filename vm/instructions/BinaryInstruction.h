#pragma once
#include "../vm.h"
#include "Instruction.h"
#include "types/value.h"

class VM;

template <typename>
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
struct ModOp
{
	Value operator()(const Value& a, const Value& b) const { return a % b; }
};
struct EqualOp
{
	Value operator()(const Value& a, const Value& b) const { return a == b; }
};
struct NotEqualOp
{
	Value operator()(const Value& a, const Value& b) const { return a != b; }
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

template <typename Op>
InterpretResult BinaryInstruction<Op>::Execute(VM& vm) const
{
	Value b = vm.Pop();
	Value a = vm.Pop();
	const Value result = Op()(a, b);
	if (result.IsNull())
	{
		return InterpretResult::RUNTIME_ERROR;
	}
	vm.Push(result);
	return InterpretResult::OK;
}