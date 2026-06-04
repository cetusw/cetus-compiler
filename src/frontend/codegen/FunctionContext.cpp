#include "FunctionContext.h"

FunctionContext::FunctionContext(std::shared_ptr<ObjFunction> function, std::optional<std::string>& error)
	: m_function(std::move(function))
	, m_emitter(m_function->chunk, error)
{
}

std::shared_ptr<ObjFunction> FunctionContext::GetFunction() const
{
	return m_function;
}

ObjFunction& FunctionContext::Function() const
{
	return *m_function;
}

BytecodeEmitter& FunctionContext::Emitter()
{
	return m_emitter;
}
