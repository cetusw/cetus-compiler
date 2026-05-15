#pragma once

#include "src/frontend/syntax/grammar/Grammar.h"
#include "src/frontend/syntax/lalr/types/LALR.h"

class ParseTablePresenter
{
public:
	virtual ~ParseTablePresenter() = default;

	virtual void Present(const ParseTable& table, const Grammar& grammar) const = 0;
};
