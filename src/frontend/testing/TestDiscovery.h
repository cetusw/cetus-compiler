#pragma once

#include "src/frontend/semantic/rules/TypeCheckResult.h"
#include "src/frontend/syntax/ast/ASTNode.h"
#include <string>
#include <unordered_set>
#include <vector>

struct TestDiscoveryResult
{
	std::vector<const TestDeclarationASTNode*> tests;
	std::unordered_set<std::string> testNames;
	std::vector<SemanticDiagnostic> diagnostics;
	std::vector<std::string> importedModules;
};

class TestDiscovery
{
public:
	[[nodiscard]] static TestDiscoveryResult Discover(const ProgramASTNode& program);
};
