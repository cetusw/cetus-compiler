#pragma once

#include "src/frontend/parsergen/grammar/Grammar.h"

// TODO больше трансформаций не будет, то и нахуй не нужен этот класс
class GrammarTransformation
{
public:
	virtual ~GrammarTransformation() = default;

	[[nodiscard]] virtual Grammar Apply(const Grammar& grammar) const = 0;
};
