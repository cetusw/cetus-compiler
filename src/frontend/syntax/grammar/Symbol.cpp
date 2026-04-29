#include "Symbol.h"
#include <utility>

Symbol::Symbol(std::string value, const bool isTerminal)
	: m_value(std::move(value))
	, m_isTerminal(isTerminal)
{
}

Symbol::Symbol()
	= default;

bool Symbol::operator<(const Symbol& other) const
{
	if (m_value != other.m_value)
	{
		return m_value < other.m_value;
	}
	return m_isTerminal < other.m_isTerminal;
}

bool Symbol::operator==(const Symbol& other) const
{
	return m_value == other.m_value && m_isTerminal == other.m_isTerminal;
}

bool Symbol::operator!=(const Symbol& other) const
{
	return !(*this == other);
}

std::string Symbol::GetValue() const
{
	return m_value;
}

bool Symbol::IsTerminal() const
{
	return m_isTerminal;
}

bool Symbol::IsEpsilon() const
{
	return m_value == "e" || m_value == "ε";
}