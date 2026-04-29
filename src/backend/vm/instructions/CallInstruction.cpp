#include "CallInstruction.h"
#include "../vm.h"
#include "src/backend/vm/objects/ObjNative.h"

InterpretResult CallInstruction::Execute(VM& vm) const
{
	const uint8_t argCount = vm.ReadByte();
	const Value callee = vm.Peek(argCount);

	if (callee.IsNative())
	{
		const auto native = callee.AsNative();
		const Value result = native->m_function(argCount, vm.GetStackTop() - argCount);
		vm.SetStackTop(vm.GetStackTop() - argCount - 1);
		vm.Push(result);
		return InterpretResult::OK;
	}
	if (callee.IsFunction())
	{
		const auto function = callee.AsFunction();

		if (argCount != function->arity)
		{
			std::fprintf(stderr, "Expected %d arguments but got %d.\n", function->arity, argCount);
			return InterpretResult::RUNTIME_ERROR;
		}

		if (vm.GetFrameCount() >= FRAMES_MAX)
		{
			std::fprintf(stderr, "Stack overflow.\n");
			return InterpretResult::RUNTIME_ERROR;
		}

		CallFrame frame;
		frame.function = function;
		frame.ip = function->chunk.GetCode().data();
		frame.slots = vm.GetStackTop() - argCount - 1;

		vm.SetFrame(vm.GetFrameCount(), frame);
		vm.SetFrameCount(vm.GetFrameCount() + 1);

		return InterpretResult::OK;
	}

	std::fprintf(stderr, "Can only call functions.\n");
	return InterpretResult::RUNTIME_ERROR;
}
