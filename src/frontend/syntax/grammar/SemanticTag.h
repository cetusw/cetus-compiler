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
	IDENTIFIER,
	MEMBER_ACCESS,
	INDEX_ACCESS
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
	if (value == "identifier") return SemanticTag::IDENTIFIER;
	if (value == "member_access") return SemanticTag::MEMBER_ACCESS;
	if (value == "index_access") return SemanticTag::INDEX_ACCESS;
	throw std::runtime_error("Unsupported semantic tag: " + std::string(value));
}
