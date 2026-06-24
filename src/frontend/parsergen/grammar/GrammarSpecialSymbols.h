#pragma once

#include "src/frontend/syntax/table/Symbol.h"

[[nodiscard]] inline Symbol EpsilonSymbol()
{
	return { "e", true };
}

[[nodiscard]] inline Symbol EofSymbol()
{
	return { "⊥", true };
}
