#include "Production.h"

#include <utility>

Production::Production(Symbol lhs, std::vector<Symbol> rhs)
	: m_lhs(std::move(lhs))
	, m_rhs(std::move(rhs))
{
}

Symbol Production::GetLhs() const
{
	return m_lhs;
}

std::vector<Symbol> Production::GetRhs() const
{
	return m_rhs;
}

std::string Production::ToString() const
{
	std::string result = m_lhs.GetValue() + " -> ";
	if (m_rhs.empty())
	{
		result += "ε";
	}
	else
	{
		for (const auto& symbol : m_rhs)
		{
			result += symbol.GetValue();
		}
	}
	return result;
}

bool Production::IsChainRule() const
{
	return m_rhs.size() == 1 && !m_rhs[0].IsTerminal();
}
