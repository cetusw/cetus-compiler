#include "vm.h"

#include "NativeRegistry.h"
#include "disassembler/Disassembler.h"
#include "instructions/InstructionRegistry.h"

VM::VM()
	: m_registry(std::make_unique<InstructionRegistry>())
	, m_frames{}
	, m_frameCount(0)
{
	m_stackTop = m_stack;

	NativeRegistry::RegisterStdLib(*this);
}

VM::~VM() = default;

void VM::LoadProgram(const Program& program)
{
	for (const std::shared_ptr<ObjFunction>& function : program.functions)
	{
		if (function && function->name)
		{
			RegisterImmutableGlobal(function->name->GetData(), Value(function));
		}
	}
}

InterpretResult VM::InterpretFunction(const std::shared_ptr<ObjFunction>& function)
{
	if (!function)
	{
		return InterpretResult::RUNTIME_ERROR;
	}

	ClearRuntimeDiagnostics();
	m_stackTop = m_stack;
	m_frameCount = 0;
	Push(Value(function));
	CallFrame frame;
	frame.function = function;
	frame.ip = function->chunk.GetCode().data();
	frame.slots = m_stack;

	m_frames[0] = frame;
	m_frameCount = 1;

	return Run();
}

InterpretResult VM::InterpretProgram(const Program& program)
{
	LoadProgram(program);
	for (const std::shared_ptr<ObjFunction>& function : program.functions)
	{
		if (!function || !function->name)
		{
			return InterpretResult::RUNTIME_ERROR;
		}
	}

	return InterpretFunction(program.entryPoint);
}

uint8_t VM::ReadByte()
{
	return *GetCurrentFrame().ip++;
}

Value VM::ReadConstant()
{
	return GetCurrentFrame().function->chunk.GetConstant(ReadByte());
}

void VM::Jump(const int offset)
{
	GetCurrentFrame().ip += offset;
}

uint16_t VM::ReadShort()
{
	auto& frame = GetCurrentFrame();
	frame.ip += 2;
	return static_cast<uint16_t>(frame.ip[-2] << 8 | frame.ip[-1]);
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

		if (result == InterpretResult::OK_DONE)
		{
			return InterpretResult::OK;
		}
		if (result != InterpretResult::OK)
		{
			return result;
		}
	}
}

void VM::TraceExecution()
{
#ifdef DEBUG_TRACE_EXECUTION
	if (m_frameCount == 0)
	{
		return;
	}

	for (const Value* slot = m_stack; slot < m_stackTop; slot++)
	{
		std::printf("[ ");
		slot->Print();
		std::printf(" ]");
	}
	std::printf("\n");

	const CallFrame& frame = GetCurrentFrame();
	const int offset = static_cast<int>(frame.ip - frame.function->chunk.GetCode().data());
	disassembler::DisassembleInstruction(frame.function->chunk, offset);
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

void VM::DefineGlobal(const std::string& name, const Value& value)
{
	m_mutableGlobals[name] = value;
}

void VM::RegisterImmutableGlobal(const std::string& name, const Value& value)
{
	m_immutableGlobals[name] = value;
}

bool VM::HasGlobal(const std::string& name) const
{
	return m_mutableGlobals.contains(name) || m_immutableGlobals.contains(name);
}

VM::GlobalSnapshot VM::SnapshotMutableGlobals() const
{
	GlobalSnapshot snapshot;
	HeapCloneCache cache;
	for (const auto& [name, value] : m_mutableGlobals)
	{
		snapshot.emplace(name, value.Clone(cache));
	}
	return snapshot;
}

void VM::RestoreMutableGlobals(GlobalSnapshot globals)
{
	m_mutableGlobals = std::move(globals);
}

Value VM::GetStack(const int index)
{
	return GetCurrentFrame().slots[index];
}

Value* VM::GetStackAddress(const int index)
{
	return &GetCurrentFrame().slots[index];
}

Value* VM::GetStackTop() const
{
	return m_stackTop;
}

CallFrame VM::GetFrame(const int index) const
{
	return m_frames[index];
}

CallFrame& VM::GetCurrentFrame()
{
	return m_frames[m_frameCount - 1];
}

int VM::GetFrameCount() const
{
	return m_frameCount;
}

Value VM::GetGlobal(const std::string& name) const
{
	const auto mutableIt = m_mutableGlobals.find(name);
	if (mutableIt != m_mutableGlobals.end())
	{
		return mutableIt->second;
	}
	return m_immutableGlobals.at(name);
}

Value* VM::GetGlobalAddress(const std::string& name)
{
	const auto it = m_mutableGlobals.find(name);
	if (it == m_mutableGlobals.end())
	{
		return nullptr;
	}
	return &it->second;
}

const std::optional<std::string>& VM::GetActiveTestName() const
{
	return m_activeTestName;
}

std::vector<std::string> VM::ConsumeRuntimeDiagnostics()
{
	std::vector<std::string> diagnostics = std::move(m_runtimeDiagnostics);
	m_runtimeDiagnostics.clear();
	return diagnostics;
}

void VM::SetStack(const int index, const Value& value)
{
	GetCurrentFrame().slots[index] = value;
}

void VM::SetStackTop(Value* value)
{
	m_stackTop = value;
}

void VM::SetFrame(const int index, const CallFrame& frame)
{
	m_frames[index] = frame;
}

void VM::SetFrameCount(const int count)
{
	m_frameCount = count;
}

bool VM::SetGlobal(const std::string& name, const Value& value)
{
	if (!m_mutableGlobals.contains(name))
	{
		return false;
	}
	m_mutableGlobals[name] = value;
	return true;
}

void VM::SetActiveTestName(std::optional<std::string> testName)
{
	m_activeTestName = std::move(testName);
}

void VM::AddRuntimeDiagnostic(std::string diagnostic)
{
	m_runtimeDiagnostics.push_back(std::move(diagnostic));
}

void VM::ClearRuntimeDiagnostics()
{
	m_runtimeDiagnostics.clear();
}
