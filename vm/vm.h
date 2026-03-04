#pragma once

#include "chunk.h"
#include "value.h"

enum class InterpretResult
{
	OK,
	COMPILE_ERROR,
	RUNTIME_ERROR
};

class VM
{
public:
	VM();
	~VM() = default;

	InterpretResult Interpret(const Chunk& chunk);

private:
	Value m_stack[STACK_MAX];
	Value* m_stackTop;
	const uint8_t* m_ip;
	const Chunk* m_chunk;

	InterpretResult Run();

	void Push(Value value);
	Value Pop();
};
