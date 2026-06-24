#include "AugmentStartGrammarTransformation.h"

Grammar AugmentStartGrammarTransformation::Apply(const Grammar& grammar) const
{
	const Symbol originalStartSymbol = grammar.GetStartSymbol();
	const Symbol augmentedStartSymbol(originalStartSymbol.GetValue() + "'", false);

	Grammar augmentedGrammar;
	augmentedGrammar.SetStartSymbol(augmentedStartSymbol);
	augmentedGrammar.AddRule(augmentedStartSymbol, { originalStartSymbol });

	for (const Production& rule : grammar.GetRules())
	{
		augmentedGrammar.AddRule(rule.GetLhs(), rule.GetRhs(), rule.GetSemanticTag());
	}

	return augmentedGrammar;
}
