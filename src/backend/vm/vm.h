#pragma once
#include "Common.h"
#include "types/CallFrame.h"
#include "types/InterpretResult.h"
#include "types/Value.h"
#include <unordered_map>

class InstructionRegistry;
class VM
{
public:
	VM();
	~VM();

	InterpretResult Interpret(const std::shared_ptr<ObjFunction>& function);
	uint8_t ReadByte();
	Value ReadConstant();

	void Jump(int offset);
	uint16_t ReadShort();

	void Push(const Value& value);
	Value Pop();
	[[nodiscard]] Value Peek(int distance) const;

	void DefineGlobal(const std::string& name, const Value& value);
	bool HasGlobal(const std::string& name) const;

	[[nodiscard]] Value GetStack(int index);
	[[nodiscard]] Value* GetStackTop() const;
	[[nodiscard]] CallFrame GetFrame(int index) const;
	[[nodiscard]] CallFrame& GetCurrentFrame();
	[[nodiscard]] int GetFrameCount() const;
	Value GetGlobal(const std::string& name) const;

	void SetStack(int index, const Value& value);
	void SetStackTop(Value* value);
	void SetFrame(int index, const CallFrame& frame);
	void SetFrameCount(int count);
	bool SetGlobal(const std::string& name, const Value& value);


private:
	Value m_stack[STACK_MAX];
	Value* m_stackTop;
	std::unique_ptr<InstructionRegistry> m_registry;
	CallFrame m_frames[FRAMES_MAX];
	int m_frameCount;
	std::unordered_map<std::string, Value> m_globals;

	InterpretResult Run();
	void TraceExecution();
};
