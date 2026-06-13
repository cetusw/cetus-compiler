#pragma once
#include "Common.h"
#include "types/CallFrame.h"
#include "types/InterpretResult.h"
#include "types/Program.h"
#include "types/Value.h"
#include <optional>
#include <unordered_map>

class InstructionRegistry;
class VM
{
public:
	using GlobalSnapshot = std::unordered_map<std::string, Value>;

	VM();
	~VM();

	InterpretResult InterpretFunction(const std::shared_ptr<ObjFunction>& function);
	InterpretResult InterpretProgram(const Program& program, bool runTests = true);
	uint8_t ReadByte();
	Value ReadConstant();

	void Jump(int offset);
	uint16_t ReadShort();

	void Push(const Value& value);
	Value Pop();
	[[nodiscard]] Value Peek(int distance) const;

	void DefineGlobal(const std::string& name, const Value& value);
	void RegisterImmutableGlobal(const std::string& name, const Value& value);
	bool HasGlobal(const std::string& name) const;
	[[nodiscard]] GlobalSnapshot SnapshotMutableGlobals() const;
	void RestoreMutableGlobals(GlobalSnapshot globals);

	[[nodiscard]] Value GetStack(int index);
	[[nodiscard]] Value* GetStackAddress(int index);
	[[nodiscard]] Value* GetStackTop() const;
	[[nodiscard]] CallFrame GetFrame(int index) const;
	[[nodiscard]] CallFrame& GetCurrentFrame();
	[[nodiscard]] int GetFrameCount() const;
	Value GetGlobal(const std::string& name) const;
	Value* GetGlobalAddress(const std::string& name);
	[[nodiscard]] const std::optional<std::string>& GetActiveTestName() const;

	void SetStack(int index, const Value& value);
	void SetStackTop(Value* value);
	void SetFrame(int index, const CallFrame& frame);
	void SetFrameCount(int count);
	bool SetGlobal(const std::string& name, const Value& value);
	void SetActiveTestName(std::optional<std::string> testName);


private:
	Value m_stack[STACK_MAX];
	Value* m_stackTop;
	std::unique_ptr<InstructionRegistry> m_registry;
	CallFrame m_frames[FRAMES_MAX];
	int m_frameCount;
	std::unordered_map<std::string, Value> m_immutableGlobals;
	std::unordered_map<std::string, Value> m_mutableGlobals;
	std::optional<std::string> m_activeTestName;

	InterpretResult Run();
	void TraceExecution();
};
