#pragma once

#include "src/frontend/ast/building/AstSemanticValue.h"
#include <vector>

class ParserStack
{
public:
	ParserStack();

	[[nodiscard]] int CurrentState() const;
	void PushState(int state);
	void PopStates(std::size_t count);

	void PushSemanticValue(AstSemanticValue value);
	[[nodiscard]] std::vector<AstSemanticValue> PopSemanticValues(std::size_t count);
	[[nodiscard]] AstSemanticValue& TopSemanticValue();
	[[nodiscard]] bool CanReduce(std::size_t rhsSize) const;

private:
	std::vector<int> m_stateStack;
	std::vector<AstSemanticValue> m_semanticStack;
};
