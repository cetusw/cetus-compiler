#pragma once

#include "types/InterpretResult.h"
#include "types/chunk.h"
#include "types/value.h"

class InstructionRegistry;
class VM
{
public:
	VM();
	~VM();

	InterpretResult Interpret(const Chunk& chunk);
	uint8_t ReadByte();
	Value ReadConstant();

	void Push(Value value);
	Value Pop();
	Value Peek(int distance) const;

private:
	Value m_stack[STACK_MAX];
	Value* m_stackTop;
	const uint8_t* m_ip;
	const Chunk* m_chunk;
	std::unique_ptr<InstructionRegistry> m_registry;

	InterpretResult Run();
	void TraceExecution() const;
};
