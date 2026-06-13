#include "ProgramContext.h"

#include "src/backend/vm/objects/ObjString.h"

constexpr std::string ENTRY_POINT_NAME = "main";

void ProgramContext::Reset()
{
	m_functions.clear();
	m_entryPoint.reset();
	m_testFunctions.clear();
}

void ProgramContext::AddFunction(std::shared_ptr<ObjFunction> function)
{
	if (function->name->GetData() == ENTRY_POINT_NAME)
	{
		m_entryPoint = function;
	}

	m_functions.push_back(std::move(function));
}

void ProgramContext::AddTestFunction(std::shared_ptr<ObjFunction> function)
{
	m_functions.push_back(function);
	m_testFunctions.push_back(std::move(function));
}

bool ProgramContext::HasEntryPoint() const
{
	return m_entryPoint != nullptr;
}

Program ProgramContext::Build()
{
	return Program{ std::move(m_functions), m_entryPoint, std::move(m_testFunctions) };
}
