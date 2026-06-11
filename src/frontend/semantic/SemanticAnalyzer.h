#pragma once

#include "rules/TypeCheckResult.h"
#include "src/frontend/syntax/ast/ASTNode.h"
#include "symbols/SymbolTable.h"
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

// TODO разделить на несколько классов
class SemanticAnalyzer final : public ASTNodeVisitor
{
public:
	explicit SemanticAnalyzer(SymbolTable symbols);

	[[nodiscard]] TypeCheckResult Analyze(const ASTNode& node);

	void Visit(const BoolLiteralASTNode& node) override;
	void Visit(const IntLiteralASTNode& node) override;
	void Visit(const FloatLiteralASTNode& node) override;
	void Visit(const StringLiteralASTNode& node) override;
	void Visit(const IdentifierASTNode& node) override;
	void Visit(const AddressOfASTNode& node) override;
	void Visit(const UnaryASTNode& node) override;
	void Visit(const BinaryASTNode& node) override;
	void Visit(const MemberAccessASTNode& node) override;
	void Visit(const IndexASTNode& node) override;
	void Visit(const CallExpressionASTNode& node) override;
	void Visit(const AssignmentASTNode& node) override;
	void Visit(const ShortVariableDeclarationASTNode& node) override;
	void Visit(const VariableDeclarationASTNode& node) override;
	void Visit(const ExpressionStatementASTNode& node) override;
	void Visit(const ProgramASTNode& node) override;
	void Visit(const StatementListASTNode& node) override;
	void Visit(const BlockASTNode& node) override;
	void Visit(const IfASTNode& node) override;
	void Visit(const ForASTNode& node) override;
	void Visit(const BreakASTNode& node) override;
	void Visit(const ContinueASTNode& node) override;
	void Visit(const ReturnASTNode& node) override;
	void Visit(const FunctionDeclarationASTNode& node) override;
	void Visit(const StructDeclarationASTNode& node) override;

private:
	[[nodiscard]] TypeDescriptor AnalyzeChild(const ASTNode& node);
	[[nodiscard]] std::vector<TypeDescriptor> AnalyzeValues(const std::vector<ASTNodePtr>& values);
	[[nodiscard]] static bool IsFalsey(TypeDescriptor type);
	[[nodiscard]] static bool HasError(const std::vector<TypeDescriptor>& types);
	void ValidateAssignment(const std::vector<ASTNodePtr>& targets, const std::vector<TypeDescriptor>& valueTypes);
	[[nodiscard]] TypeDescriptor AnalyzeAssignmentTarget(const ASTNode& target);
	void DefineShortVariables(const std::vector<std::string>& names, const std::vector<TypeDescriptor>& valueTypes);
	void DefineVariables(
		const std::vector<std::string>& names,
		const std::optional<TypeDescriptor>& declaredType,
		const std::vector<TypeDescriptor>& valueTypes);
	void PredeclareTopLevelFunctions(const ASTNode& node);
	void PredeclareTopLevelFunctions(const StatementListASTNode& node);
	void PredeclareFunction(const FunctionDeclarationASTNode& node);
	void PredeclareTopLevelTypes(const ASTNode& node);
	void PredeclareTopLevelTypes(const StatementListASTNode& node);
	void ValidateEntryPoint();
	void DefineBuiltinFunctions();
	void TypeCheckBuiltinCall(const CallExpressionASTNode& node, const std::vector<TypeDescriptor>& argumentTypes);
	void TypeCheckFunctionCall(const CallExpressionASTNode& node, const SemanticSymbol& symbol, const std::vector<TypeDescriptor>& argumentTypes);
	void TypeCheckMethodCall(const CallExpressionASTNode& node, const TypeDescriptor& receiverType, const MethodSignature& method, const std::vector<TypeDescriptor>& argumentTypes);
	[[nodiscard]] bool ValidateValueExpression(const TypeDescriptor& type, const char* context);
	[[nodiscard]] bool ValidateTypeReference(const TypeDescriptor& type, const char* context);
	[[nodiscard]] bool ValidateStructFields(const StructDeclarationASTNode& node);
	[[nodiscard]] const FieldSignature* ResolveField(const TypeDescriptor& objectType, const std::string& fieldName) const;
	[[nodiscard]] const MethodSignature* ResolveMethod(const TypeDescriptor& objectType, const std::string& methodName) const;
	[[nodiscard]] bool ValidateUserDefinedName(const std::string& name, const char* declarationKind);
	[[nodiscard]] bool DefineFunctionSymbol(const FunctionDeclarationASTNode& node);
	[[nodiscard]] bool DefineTypeSymbol(const StructDeclarationASTNode& node);
	[[nodiscard]] static std::vector<ParameterSignature> BuildParameterSignatures(const FunctionDeclarationASTNode& node);
	[[nodiscard]] static std::vector<ParameterSignature> BuildCallableParameterSignatures(const FunctionDeclarationASTNode& node);
	[[nodiscard]] static std::vector<FieldSignature> BuildFieldSignatures(const StructDeclarationASTNode& node);
	[[nodiscard]] static bool AlwaysReturns(const ASTNode& node);
	[[nodiscard]] static bool StatementListAlwaysReturns(const StatementListASTNode& node);
	[[nodiscard]] static bool IfAlwaysReturns(const IfASTNode& node);
	[[nodiscard]] static bool IsCallableKind(SemanticSymbolKind kind);
	void SetCurrentType(const ASTNode& node, const TypeDescriptor& type);
	void SetTypeCheckResult(const ASTNode& node, TypeCheckResult result);
	void AddDiagnostic(std::string message);
	void AddDiagnostics(std::vector<SemanticDiagnostic> diagnostics);

	SymbolTable m_symbolTable;
	std::optional<TypeDescriptor> m_currentFunctionReturnType;
	TypeDescriptor m_currentType = Type::ERROR;
	std::vector<SemanticDiagnostic> m_diagnostics;
	std::unordered_set<const FunctionDeclarationASTNode*> m_predeclaredFunctions;
	std::unordered_set<const StructDeclarationASTNode*> m_predeclaredTypes;
	int m_loopDepth = 0;
};
