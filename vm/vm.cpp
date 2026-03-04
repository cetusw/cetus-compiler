#include "vm.h"

VM::VM()
	: m_ip(nullptr)
	, m_chunk(nullptr)
{
	m_stackTop = m_stack;
}

InterpretResult VM::Interpret(const Chunk& chunk)
{
	m_chunk = &chunk;
	m_ip = chunk.GetCode().data();
	return Run();
}

InterpretResult VM::Run()
{
#define READ_BYTE() (*m_ip++)
#define READ_CONSTANT() (m_chunk->GetConstant(READ_BYTE()))
	for (;;)
	{
		uint8_t instruction = READ_BYTE();

		switch (static_cast<OpCode>(instruction))
		{
		case OP_CONSTANT:
		{
			const Value constant = READ_CONSTANT();
			Push(constant);
			break;
		}
		case OP_RETURN:
		{
			return InterpretResult::OK;
		}
		}
	}
#undef READ_CONSTANT
#undef READ_BYTE
}

void VM::Push(const Value value)
{
	*m_stackTop = value;
	m_stackTop++;
}

Value VM::Pop()
{
	m_stackTop--;
	return *m_stackTop;
}