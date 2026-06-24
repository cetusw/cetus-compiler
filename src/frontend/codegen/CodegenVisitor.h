#pragma once

#include "../semantic/rules/TypeCheckResult.h"
#include "../semantic/symbols/SymbolTable.h"
#include "CodegenResult.h"
#include "FunctionContext.h"
#include "ProgramContext.h"
#include "src/frontend/ast/ASTNode.h"
#include <vector>

class ASTNode;

class CodegenVisitor final : public ASTNodeVisitor
{
public:
	CodegenVisitor(const SymbolTable& symbols, const TypeCheckResult& typeInfo);

	[[nodiscard]] CodegenResult Generate(const ASTNode& expr);

	void Visit(const BoolLiteralASTNode& expr) override;
	void Visit(const NilLiteralASTNode& expr) override;
	void Visit(const IntLiteralASTNode& expr) override;
	void Visit(const FloatLiteralASTNode& expr) override;
	void Visit(const StringLiteralASTNode& expr) override;
	void Visit(const ArrayLiteralASTNode& expr) override;
	void Visit(const StructLiteralASTNode& expr) override;
	void Visit(const IdentifierASTNode& expr) override;
	void Visit(const AddressOfASTNode& expr) override;
	void Visit(const UnaryASTNode& expr) override;
	void Visit(const BinaryASTNode& expr) override;
	void Visit(const MemberAccessASTNode& expr) override;
	void Visit(const IndexASTNode& expr) override;
	void Visit(const SliceExpressionASTNode& expr) override;
	void Visit(const CallExpressionASTNode& expr) override;
	void Visit(const AssignmentASTNode& expr) override;
	void Visit(const IncrementASTNode& expr) override;
	void Visit(const DecrementASTNode& expr) override;
	void Visit(const ShortVariableDeclarationASTNode& expr) override;
	void Visit(const VariableDeclarationASTNode& expr) override;
	void Visit(const ExpressionStatementASTNode& expr) override;
	void Visit(const AssertStatementASTNode& expr) override;
	void Visit(const ForAllStatementASTNode& expr) override;
	void Visit(const EmptyStatementASTNode& expr) override;
	void Visit(const ProgramASTNode& expr) override;
	void Visit(const StatementListASTNode& expr) override;
	void Visit(const BlockASTNode& expr) override;
	void Visit(const IfASTNode& expr) override;
	void Visit(const ForASTNode& expr) override;
	void Visit(const BreakASTNode& expr) override;
	void Visit(const ContinueASTNode& expr) override;
	void Visit(const ReturnASTNode& expr) override;
	void Visit(const FunctionDeclarationASTNode& expr) override;
	void Visit(const StructDeclarationASTNode& expr) override;
	void Visit(const TestDeclarationASTNode& expr) override;

private:
	[[nodiscard]] BytecodeEmitter& CurrentEmitter();
	void Fail(std::string message);
	void EmitDefault(const TypeDescriptor& type);
	void EmitStructDefault(const TypeDescriptor& type);
	void EmitScalarDefault(Type type);
	[[nodiscard]] std::shared_ptr<ObjFunction> BuildCallable(
		const std::string& name,
		int arity,
		int returnArity,
		const ASTNode& body,
		const std::vector<FunctionParameter>* parameters = nullptr,
		const FunctionParameter* receiver = nullptr);
	void EmitAssignmentTarget(const ASTNode& target);
	void EmitBinaryOperation(BinaryOperator op);
	void EmitLogicalAnd(const BinaryASTNode& expr);
	void EmitLogicalOr(const BinaryASTNode& expr);
	int EmitExpandedValues(const std::vector<ASTNodePtr>& values);
	int GetReturnArity(const TypeDescriptor& type) const;
	void EmitPopForType(const TypeDescriptor& type);
	void EmitScopeCleanup(int scopeDepth);
	void EmitIdentifierRef(const IdentifierASTNode& expr);
	void EmitAddressableRef(const ASTNode& expr);
	[[nodiscard]] const MethodSignature* ResolveMethod(const TypeDescriptor& receiverType, const std::string& methodName) const;
	[[nodiscard]] bool ShouldPassArgumentByPointer(const std::string& calleeName, std::size_t argumentIndex) const;
	[[nodiscard]] bool EnsureTyped(const ASTNode& expr);

	struct LoopContext
	{
		std::vector<int> breakJumps;
		std::vector<int> continueJumps;
		int breakScopeDepth = 0;
		int continueScopeDepth = 0;
		int continueTarget = 0;
		bool continueJumpsForward = false;
	};

	const SymbolTable& m_symbols;
	const TypeCheckResult& m_typeInfo;
	ProgramContext m_programContext;
	std::vector<FunctionContext> m_functionStack;
	std::vector<LoopContext> m_loopStack;
	std::optional<std::string> m_error;
	std::optional<std::string> m_activeTestName;
	int m_activeTestPropertyIndex = 0;
	std::vector<PropertyDescriptor> m_activeTestProperties;
};
