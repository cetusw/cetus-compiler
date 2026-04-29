#pragma once

#include "Symbol.h"
#include <vector>

class Production
{
public:
	Production(Symbol lhs, std::vector<Symbol> rhs);

	[[nodiscard]] const Symbol& GetLhs() const;
	[[nodiscard]] const std::vector<Symbol>& GetRhs() const;
	[[nodiscard]] std::string ToString() const;
	[[nodiscard]] bool IsChainRule() const;

private:
	Symbol m_lhs;
	std::vector<Symbol> m_rhs;
};
