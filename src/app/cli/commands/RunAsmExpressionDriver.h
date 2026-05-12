#pragma once

#include "src/app/cli/CommandLineInterface.h"
#include "src/frontend/codegen/AsmCodegenResult.h"
#include "src/frontend/syntax/ast/Expr.h"
#include "src/frontend/syntax/semantic/TypeCheckResult.h"

class RunAsmExpressionDriver
{
public:
	static void Execute(const Configuration& configuration);

private:
	[[nodiscard]] static ExprPtr ParseSourceFile(const Configuration& configuration);
	[[nodiscard]] static TypeCheckResult TypeCheckAst(const Expr& ast);
	[[nodiscard]] static AsmCodegenResult GenerateAssembly(const Expr& ast, const TypeCheckResult& typeResult);
	static void WriteAssemblyOutput(const Configuration& configuration, const AsmCodegenResult& codegenResult);
};
