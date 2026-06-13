#include "TestCoverageAnalyzer.h"

namespace
{
const StatementListASTNode* AsStatementList(const ASTNode& node)
{
	return dynamic_cast<const StatementListASTNode*>(&node);
}
}

// TODO отрефакторить
std::vector<SemanticDiagnostic> TestCoverageAnalyzer::Analyze(
	const ProgramASTNode& program,
	const TestDiscoveryResult& tests)
{
	std::vector<SemanticDiagnostic> diagnostics;

	const StatementListASTNode* statements = AsStatementList(program.GetStatements());
	if (!statements)
	{
		diagnostics.push_back({ "Program statements are not represented as StatementListASTNode.", 0 });
		return diagnostics;
	}

	for (const ASTNodePtr& child : statements->GetStatements())
	{
		const auto* function = dynamic_cast<const FunctionDeclarationASTNode*>(child.get());
		if (!function)
		{
			continue;
		}

		if (!function->IsMethod() && function->GetName() == "main")
		{
			continue;
		}

		const std::string requiredTestName = function->IsMethod()
			? function->GetQualifiedName()
			: function->GetName();
		if (tests.testNames.contains(requiredTestName))
		{
			continue;
		}

		if (function->IsMethod())
		{
			diagnostics.push_back({ "Missing test for method: " + requiredTestName, 0 });
		}
		else
		{
			diagnostics.push_back({ "Missing test for function: " + requiredTestName, 0 });
		}
	}

	return diagnostics;
}
