#pragma once

#include "GrammarSets.h"
#include "src/frontend/parsergen/grammar/Grammar.h"

class GrammarSetsBuilder
{
public:
	[[nodiscard]] static GrammarSets Build(const Grammar& grammar);
};
