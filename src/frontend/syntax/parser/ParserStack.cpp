#include "ParserStack.h"

#include <stdexcept>

ParserStack::ParserStack()
	: m_stateStack{ 0 }
{
}

int ParserStack::CurrentState() const
{
	return m_stateStack.back();
}

void ParserStack::PushState(const int state)
{
	m_stateStack.push_back(state);
}

void ParserStack::PopStates(const std::size_t count)
{
	if (m_stateStack.size() <= count)
	{
		throw std::logic_error("Parser state stack underflow during reduce.");
	}

	for (std::size_t index = 0; index < count; ++index)
	{
		m_stateStack.pop_back();
	}
}

void ParserStack::PushSemanticValue(AstSemanticValue value)
{
	m_semanticStack.push_back(std::move(value));
}

std::vector<AstSemanticValue> ParserStack::PopSemanticValues(const std::size_t count)
{
	if (m_semanticStack.size() < count)
	{
		throw std::logic_error("Parser semantic stack underflow during reduce.");
	}

	std::vector<AstSemanticValue> values(count);
	for (std::size_t index = 0; index < count; ++index)
	{
		values[count - 1 - index] = std::move(m_semanticStack.back());
		m_semanticStack.pop_back();
	}
	return values;
}

AstSemanticValue& ParserStack::TopSemanticValue()
{
	if (m_semanticStack.empty())
	{
		throw std::logic_error("Parser semantic stack is empty.");
	}
	return m_semanticStack.back();
}

bool ParserStack::CanReduce(const std::size_t rhsSize) const
{
	return m_stateStack.size() > rhsSize && m_semanticStack.size() >= rhsSize;
}
