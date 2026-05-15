#pragma once

#include "../../../frontend/codegen/asm/AsmCodegenResult.h"
#include "../../../frontend/semantic/rules/TypeCheckResult.h"
#include "src/app/cli/CommandLineInterface.h"
#include "src/frontend/lexical/LexicalAnalyzer.h"
#include "src/frontend/syntax/ast/ASTNode.h"

class RunAsmExpressionDriver
{
public:
	static void Execute(const Configuration& configuration);

private:
	[[nodiscard]] static std::string ReadSource(const Configuration& configuration);
	[[nodiscard]] static std::vector<Token> Tokenize(const std::string& source);
	[[nodiscard]] static ASTNodePtr ParseTokens(const std::vector<Token>& tokens, bool regenerateTable);
	[[nodiscard]] static TypeCheckResult TypeCheckAst(const ASTNode& ast);
	[[nodiscard]] static AsmCodegenResult GenerateAssembly(const ASTNode& ast, const TypeCheckResult& typeResult);
	static void WriteAssemblyOutput(const Configuration& configuration, const AsmCodegenResult& codegenResult);
};
