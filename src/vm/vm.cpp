#include "vm.h"
#include "disassembler/Disassembler.h"
#include "instructions/InstructionRegistry.h"

VM::VM()
	: m_ip(nullptr)
	, m_chunk(nullptr)
	, m_registry(std::make_unique<InstructionRegistry>())
{
	m_stackTop = m_stack;
}

VM::~VM() = default;

InterpretResult VM::Interpret(const Chunk& chunk)
{
	m_chunk = &chunk;
	m_ip = chunk.GetCode().data();
	return Run();
}

inline uint8_t VM::ReadByte()
{
	if (m_ip >= m_chunk->GetCode().data() + m_chunk->GetCode().size())
	{
		throw std::out_of_range("Bytecode overread");
	}
	return *m_ip++;
}

Value VM::ReadConstant()
{
	return m_chunk->GetConstant(ReadByte());
}

void VM::Jump(const int offset)
{
	m_ip += offset;
}

uint16_t VM::ReadShort()
{
	m_ip += 2;
	return static_cast<uint16_t>(m_ip[-2] << 8 | m_ip[-1]);
}

InterpretResult VM::Run()
{
#ifdef DEBUG_TRACE_EXECUTION
	std::printf("\n--- Trace Execution ---\n");
#endif
	for (;;)
	{
		TraceExecution();
		const uint8_t opcode = ReadByte();
		const Instruction* instruction = m_registry->Get(opcode);
		if (!instruction)
		{
			return InterpretResult::RUNTIME_ERROR;
		}

		const InterpretResult result = instruction->Execute(*this);
		if (result != InterpretResult::OK)
		{
			return result == InterpretResult::OK_DONE ? InterpretResult::OK : result;
		}
	}
}

void VM::TraceExecution() const
{
#ifdef DEBUG_TRACE_EXECUTION
	for (const Value* slot = m_stack; slot < m_stackTop; slot++)
	{
		std::printf("[ ");
		slot->Print();
		std::printf(" ]");
	}
	std::printf("\n");

	const int offset = static_cast<int>(m_ip - m_chunk->GetCode().data());
	disassembler::DisassembleInstruction(*m_chunk, offset);
#endif
}

void VM::Push(const Value& value)
{
	*m_stackTop = value;
	m_stackTop++;
}

Value VM::Pop()
{
	m_stackTop--;
	return *m_stackTop;
}

Value VM::Peek(const int distance) const
{
	return m_stackTop[-1 - distance];
}

Value VM::GetStack(const int index) const
{
	return m_stack[index];
}

void VM::SetStack(const int index, const Value& value)
{
	m_stack[index] = value;
}