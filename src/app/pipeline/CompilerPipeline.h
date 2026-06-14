#pragma once

#include "src/app/cli/CommandLineInterface.h"
#include "src/backend/vm/vm.h"
#include "src/frontend/codegen/CodegenResult.h"
#include "src/frontend/syntax/parser/ParseResult.h"
#include "src/frontend/testing/TestDiscovery.h"

class CompilerPipeline
{
public:
	[[nodiscard]] static std::unique_ptr<ProgramASTNode> ParseFile(const Configuration& configuration);
	[[nodiscard]] static TestDiscoveryResult DiscoverTests(const ProgramASTNode& program);
	[[nodiscard]] static TypeCheckResult TypeCheck(const ProgramASTNode& program);
	static void ValidateTestCoverage(const ProgramASTNode& program, const TestDiscoveryResult& discoveryResult);
	[[nodiscard]] static CodegenResult Compile(const ProgramASTNode& program, const TypeCheckResult& typeResult);
	static void RunTests(const CodegenResult& codegenResult, VM& vm, bool report);
	static void RunProgram(const CodegenResult& codegenResult, VM& vm);
};