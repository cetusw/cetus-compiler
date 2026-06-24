#pragma once

#include "src/frontend/parsergen/grammar/transformation/GrammarTransformation.h"

class AugmentStartGrammarTransformation final : public GrammarTransformation
{
public:
	[[nodiscard]] Grammar Apply(const Grammar& grammar) const override;
};
