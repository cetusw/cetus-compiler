#include "Production.h"

#include <utility>

Production::Production(Symbol lhs, std::vector<Symbol> rhs, const SemanticTag semanticTag)
	: m_lhs(std::move(lhs))
	, m_rhs(std::move(rhs))
	, m_semanticTag(semanticTag)
{
}

const Symbol& Production::GetLhs() const
{
	return m_lhs;
}

const std::vector<Symbol>& Production::GetRhs() const
{
	return m_rhs;
}

SemanticTag Production::GetSemanticTag() const
{
	return m_semanticTag;
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
