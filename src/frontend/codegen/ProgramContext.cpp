#include "ProgramContext.h"

#include "src/backend/vm/objects/ObjString.h"

constexpr std::string ENTRY_POINT_NAME = "main";

void ProgramContext::Reset()
{
	m_functions.clear();
	m_entryPoint.reset();
	m_testManifest = {};
}

void ProgramContext::AddFunction(std::shared_ptr<ObjFunction> function)
{
	if (function->name->GetData() == ENTRY_POINT_NAME)
	{
		m_entryPoint = function;
	}

	m_functions.push_back(std::move(function));
}

void ProgramContext::AddTestFunction(
	std::string testName,
	std::shared_ptr<ObjFunction> function,
	std::vector<PropertyDescriptor> properties)
{
	m_functions.push_back(function);
	m_testManifest.coveredSymbols.insert(testName);
	m_testManifest.tests.push_back({ std::move(testName), std::move(function), std::move(properties) });
}

bool ProgramContext::HasEntryPoint() const
{
	return m_entryPoint != nullptr;
}

Program ProgramContext::Build()
{
	return Program{ std::move(m_functions), m_entryPoint, std::move(m_testManifest) };
}
