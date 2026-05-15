#pragma once

#include "src/frontend/syntax/grammar/Grammar.h"

class GrammarTransformation
{
public:
	virtual ~GrammarTransformation() = default;

	[[nodiscard]] virtual Grammar Apply(const Grammar& grammar) const = 0;
};
