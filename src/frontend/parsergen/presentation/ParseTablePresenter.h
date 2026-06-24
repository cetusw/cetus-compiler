#pragma once

#include "src/frontend/parsergen/grammar/Grammar.h"
#include "src/frontend/syntax/table/ParseTable.h"

class ParseTablePresenter
{
public:
	virtual ~ParseTablePresenter() = default;

	virtual void Present(const ParseTable&, const Grammar&) const
	{
	}
};
