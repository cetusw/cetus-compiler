#include "TestDiscovery.h"

namespace
{
const StatementListASTNode* AsStatementList(const ASTNode& node)
{
	return dynamic_cast<const StatementListASTNode*>(&node);
}
}

TestDiscoveryResult TestDiscovery::Discover(const ProgramASTNode& program)
{
	TestDiscoveryResult result;

	const StatementListASTNode* statements = AsStatementList(program.GetStatements());
	if (!statements)
	{
		result.diagnostics.push_back({ "Program statements are not represented as StatementListASTNode.", 0 });
		return result;
	}

	for (const ASTNodePtr& child : statements->GetStatements())
	{
		const auto* test = dynamic_cast<const TestDeclarationASTNode*>(child.get());
		if (!test)
		{
			continue;
		}

		// TODO уточнить
		result.tests.push_back(test);
		if (!result.testNames.insert(test->GetName()).second)
		{
			result.diagnostics.push_back({ "Test is already declared: " + test->GetName(), 0 });
		}
	}

	return result;
}
