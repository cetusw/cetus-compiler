#pragma once

#include "src/backend/vm/types/OpCode.h"
#include "src/frontend/syntax/ast/ASTNode.h"
#include <optional>

class OperatorOpcodeResolver
{
public:
	[[nodiscard]] static std::optional<OpCode> Resolve(UnaryOperator op);
	[[nodiscard]] static std::optional<OpCode> Resolve(BinaryOperator op);
};
