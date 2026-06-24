#include "SemanticTagFormatter.h"

#include <algorithm>
#include <stdexcept>

namespace
{
struct SemanticTagEntry
{
	std::string_view name;
	SemanticTag tag;
};

constexpr SemanticTagEntry SEMANTIC_TAGS[] = {
	{ "none", SemanticTag::NONE },
	{ "pass_expr", SemanticTag::PASS_EXPR },
	{ "pass_token", SemanticTag::PASS_TOKEN },
	{ "group", SemanticTag::GROUP },
	{ "unary", SemanticTag::UNARY },
	{ "binary", SemanticTag::BINARY },
	{ "bool_literal", SemanticTag::BOOL_LITERAL },
	{ "nil_literal", SemanticTag::NIL_LITERAL },
	{ "int_literal", SemanticTag::INT_LITERAL },
	{ "float_literal", SemanticTag::FLOAT_LITERAL },
	{ "string_literal", SemanticTag::STRING_LITERAL },
	{ "array_literal", SemanticTag::ARRAY_LITERAL },
	{ "array_literal_empty", SemanticTag::ARRAY_LITERAL_EMPTY },
	{ "struct_literal", SemanticTag::STRUCT_LITERAL },
	{ "struct_literal_empty", SemanticTag::STRUCT_LITERAL_EMPTY },
	{ "named_postfix", SemanticTag::NAMED_POSTFIX },
	{ "identifier_tail", SemanticTag::IDENTIFIER_TAIL },
	{ "struct_literal_tail", SemanticTag::STRUCT_LITERAL_TAIL },
	{ "struct_literal_empty_tail", SemanticTag::STRUCT_LITERAL_EMPTY_TAIL },
	{ "identifier", SemanticTag::IDENTIFIER },
	{ "address_of", SemanticTag::ADDRESS_OF },
	{ "address_of_struct_literal", SemanticTag::ADDRESS_OF_STRUCT_LITERAL },
	{ "address_of_struct_literal_empty", SemanticTag::ADDRESS_OF_STRUCT_LITERAL_EMPTY },
	{ "identifier_list", SemanticTag::IDENTIFIER_LIST },
	{ "identifier_list_single", SemanticTag::IDENTIFIER_LIST_SINGLE },
	{ "expression_list", SemanticTag::EXPRESSION_LIST },
	{ "expression_list_single", SemanticTag::EXPRESSION_LIST_SINGLE },
	{ "type_name", SemanticTag::TYPE_NAME },
	{ "array_type", SemanticTag::ARRAY_TYPE },
	{ "slice_type", SemanticTag::SLICE_TYPE },
	{ "pointer_type", SemanticTag::POINTER_TYPE },
	{ "single_return_type", SemanticTag::SINGLE_RETURN_TYPE },
	{ "tuple_return_type", SemanticTag::TUPLE_RETURN_TYPE },
	{ "type_list", SemanticTag::TYPE_LIST },
	{ "type_list_single", SemanticTag::TYPE_LIST_SINGLE },
	{ "param", SemanticTag::PARAM },
	{ "pointer_param", SemanticTag::POINTER_PARAM },
	{ "param_list", SemanticTag::PARAM_LIST },
	{ "param_list_single", SemanticTag::PARAM_LIST_SINGLE },
	{ "call_no_args", SemanticTag::CALL_NO_ARGS },
	{ "call", SemanticTag::CALL },
	{ "method_call_no_args", SemanticTag::METHOD_CALL_NO_ARGS },
	{ "method_call", SemanticTag::METHOD_CALL },
	{ "member_access", SemanticTag::MEMBER_ACCESS },
	{ "index_access", SemanticTag::INDEX_ACCESS },
	{ "slice_expression", SemanticTag::SLICE_EXPRESSION },
	{ "assignable_list", SemanticTag::ASSIGNABLE_LIST },
	{ "assignable_list_single", SemanticTag::ASSIGNABLE_LIST_SINGLE },
	{ "assignment", SemanticTag::ASSIGNMENT },
	{ "increment", SemanticTag::INCREMENT },
	{ "decrement", SemanticTag::DECREMENT },
	{ "struct_field", SemanticTag::STRUCT_FIELD },
	{ "struct_field_list", SemanticTag::STRUCT_FIELD_LIST },
	{ "struct_field_list_single", SemanticTag::STRUCT_FIELD_LIST_SINGLE },
	{ "field_initializer", SemanticTag::FIELD_INITIALIZER },
	{ "field_initializer_list", SemanticTag::FIELD_INITIALIZER_LIST },
	{ "field_initializer_list_single", SemanticTag::FIELD_INITIALIZER_LIST_SINGLE },
	{ "struct_declaration", SemanticTag::STRUCT_DECLARATION },
	{ "short_var_declaration", SemanticTag::SHORT_VAR_DECLARATION },
	{ "var_inferred_declaration", SemanticTag::VAR_INFERRED_DECLARATION },
	{ "var_typed_declaration", SemanticTag::VAR_TYPED_DECLARATION },
	{ "var_typed_initialized_declaration", SemanticTag::VAR_TYPED_INITIALIZED_DECLARATION },
	{ "var_typed_declaration_full", SemanticTag::VAR_TYPED_DECLARATION_FULL },
	{ "var_typed_composite_declaration", SemanticTag::VAR_TYPED_COMPOSITE_DECLARATION },
	{ "var_typed_empty_composite_declaration", SemanticTag::VAR_TYPED_EMPTY_COMPOSITE_DECLARATION },
	{ "expression_statement", SemanticTag::EXPRESSION_STATEMENT },
	{ "assert_statement", SemanticTag::ASSERT_STATEMENT },
	{ "forall_statement", SemanticTag::FORALL_STATEMENT },
	{ "empty_statement", SemanticTag::EMPTY_STATEMENT },
	{ "program", SemanticTag::PROGRAM },
	{ "program_empty", SemanticTag::PROGRAM_EMPTY },
	{ "statement_list", SemanticTag::STATEMENT_LIST },
	{ "statement_list_single", SemanticTag::STATEMENT_LIST_SINGLE },
	{ "if", SemanticTag::IF },
	{ "if_else", SemanticTag::IF_ELSE },
	{ "for_condition", SemanticTag::FOR_CONDITION },
	{ "for_classic", SemanticTag::FOR_CLASSIC },
	{ "break", SemanticTag::BREAK },
	{ "continue", SemanticTag::CONTINUE },
	{ "return_void", SemanticTag::RETURN_VOID },
	{ "return_value", SemanticTag::RETURN_VALUE },
	{ "test_declaration", SemanticTag::TEST_DECLARATION },
	{ "function_void_no_params", SemanticTag::FUNCTION_VOID_NO_PARAMS },
	{ "function_return_no_params", SemanticTag::FUNCTION_RETURN_NO_PARAMS },
	{ "function_void", SemanticTag::FUNCTION_VOID },
	{ "function_return", SemanticTag::FUNCTION_RETURN },
	{ "method_void_no_params", SemanticTag::METHOD_VOID_NO_PARAMS },
	{ "method_return_no_params", SemanticTag::METHOD_RETURN_NO_PARAMS },
	{ "method_void", SemanticTag::METHOD_VOID },
	{ "method_return", SemanticTag::METHOD_RETURN },
	{ "block", SemanticTag::BLOCK },
	{ "block_empty", SemanticTag::BLOCK_EMPTY },
};
} // namespace

SemanticTag ParseSemanticTagName(const std::string_view value)
{
	if (value.empty())
	{
		return SemanticTag::NONE;
	}

	const auto entry = std::ranges::find_if(SEMANTIC_TAGS,
		[value](const SemanticTagEntry& candidate) { return candidate.name == value; });
	if (entry != std::end(SEMANTIC_TAGS))
	{
		return entry->tag;
	}

	throw std::runtime_error("Unsupported semantic tag: " + std::string(value));
}

std::string FormatSemanticTagName(const SemanticTag tag)
{
	const auto entry = std::ranges::find_if(SEMANTIC_TAGS,
		[tag](const SemanticTagEntry& candidate) { return candidate.tag == tag; });
	if (entry != std::end(SEMANTIC_TAGS))
	{
		return std::string(entry->name);
	}

	return "none";
}
