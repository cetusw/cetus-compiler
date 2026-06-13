#pragma once

#include "TestDiscovery.h"
#include "src/frontend/semantic/rules/TypeCheckResult.h"
#include "src/frontend/syntax/ast/ASTNode.h"
#include <vector>

class TestCoverageAnalyzer
{
public:
	[[nodiscard]] static std::vector<SemanticDiagnostic> Analyze(
		const ProgramASTNode& program,
		const TestDiscoveryResult& tests);
};
