#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

enum class SemanticTag
{
	NONE,
	PASS_EXPR,
	PASS_TOKEN,
	GROUP,
	UNARY,
	BINARY,
	BOOL_LITERAL,
	INT_LITERAL,
	FLOAT_LITERAL,
	STRING_LITERAL,
	IDENTIFIER,
	IDENTIFIER_LIST,
	IDENTIFIER_LIST_SINGLE,
	EXPRESSION_LIST,
	EXPRESSION_LIST_SINGLE,
	TYPE_NAME,
	MEMBER_ACCESS,
	INDEX_ACCESS,
	ASSIGNMENT,
	SHORT_VAR_DECLARATION,
	VAR_INFERRED_DECLARATION,
	VAR_TYPED_DECLARATION,
	VAR_TYPED_INITIALIZED_DECLARATION,
	EXPRESSION_STATEMENT,
	PROGRAM,
	PROGRAM_EMPTY,
	STATEMENT_LIST,
	STATEMENT_LIST_SINGLE,
	IF,
	IF_ELSE,
	PRINTF,
	RETURN_VOID,
	RETURN_VALUE,
	BLOCK,
	BLOCK_EMPTY
};

[[nodiscard]] inline SemanticTag ParseSemanticTagName(const std::string_view value)
{
	if (value.empty()) return SemanticTag::NONE;
	if (value == "pass_expr") return SemanticTag::PASS_EXPR;
	if (value == "pass_token") return SemanticTag::PASS_TOKEN;
	if (value == "group") return SemanticTag::GROUP;
	if (value == "unary") return SemanticTag::UNARY;
	if (value == "binary") return SemanticTag::BINARY;
	if (value == "bool_literal") return SemanticTag::BOOL_LITERAL;
	if (value == "int_literal") return SemanticTag::INT_LITERAL;
	if (value == "float_literal") return SemanticTag::FLOAT_LITERAL;
	if (value == "string_literal") return SemanticTag::STRING_LITERAL;
	if (value == "identifier") return SemanticTag::IDENTIFIER;
	if (value == "identifier_list") return SemanticTag::IDENTIFIER_LIST;
	if (value == "identifier_list_single") return SemanticTag::IDENTIFIER_LIST_SINGLE;
	if (value == "expression_list") return SemanticTag::EXPRESSION_LIST;
	if (value == "expression_list_single") return SemanticTag::EXPRESSION_LIST_SINGLE;
	if (value == "type_name") return SemanticTag::TYPE_NAME;
	if (value == "member_access") return SemanticTag::MEMBER_ACCESS;
	if (value == "index_access") return SemanticTag::INDEX_ACCESS;
	if (value == "assignment") return SemanticTag::ASSIGNMENT;
	if (value == "short_var_declaration") return SemanticTag::SHORT_VAR_DECLARATION;
	if (value == "var_inferred_declaration") return SemanticTag::VAR_INFERRED_DECLARATION;
	if (value == "var_typed_declaration") return SemanticTag::VAR_TYPED_DECLARATION;
	if (value == "var_typed_initialized_declaration") return SemanticTag::VAR_TYPED_INITIALIZED_DECLARATION;
	if (value == "expression_statement") return SemanticTag::EXPRESSION_STATEMENT;
	if (value == "program") return SemanticTag::PROGRAM;
	if (value == "program_empty") return SemanticTag::PROGRAM_EMPTY;
	if (value == "statement_list") return SemanticTag::STATEMENT_LIST;
	if (value == "statement_list_single") return SemanticTag::STATEMENT_LIST_SINGLE;
	if (value == "if") return SemanticTag::IF;
	if (value == "if_else") return SemanticTag::IF_ELSE;
	if (value == "printf") return SemanticTag::PRINTF;
	if (value == "return_void") return SemanticTag::RETURN_VOID;
	if (value == "return_value") return SemanticTag::RETURN_VALUE;
	if (value == "block") return SemanticTag::BLOCK;
	if (value == "block_empty") return SemanticTag::BLOCK_EMPTY;
	throw std::runtime_error("Unsupported semantic tag: " + std::string(value));
}
