#pragma once

#include "src/frontend/syntax/table/SemanticTag.h"
#include "src/frontend/syntax/table/Symbol.h"
#include <string>
#include <vector>

class Production
{
public:
	Production(Symbol lhs, std::vector<Symbol> rhs, SemanticTag semanticTag = SemanticTag::NONE);

	[[nodiscard]] const Symbol& GetLhs() const;
	[[nodiscard]] const std::vector<Symbol>& GetRhs() const;
	[[nodiscard]] SemanticTag GetSemanticTag() const;
	[[nodiscard]] std::string ToString() const;
	[[nodiscard]] bool IsChainRule() const;

private:
	Symbol m_lhs;
	std::vector<Symbol> m_rhs;
	SemanticTag m_semanticTag = SemanticTag::NONE;
};
