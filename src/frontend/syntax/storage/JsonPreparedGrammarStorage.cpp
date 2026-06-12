#include "JsonPreparedGrammarStorage.h"

#include "src/frontend/syntax/lalr/ActionFormatter.h"
#include "src/support/io/FileReader.h"

#include <stdexcept>
#include <string>

PreparedGrammar JsonPreparedGrammarStorage::Load(const std::string& filePath) const
{
	std::string rawJson = FileReader::ReadAll(filePath);
	const Json parsedJson = Json::parse(rawJson);
	return ParsePreparedGrammar(parsedJson);
}

void JsonPreparedGrammarStorage::Save(const PreparedGrammar& preparedGrammar, const std::string& filePath) const
{
	const std::string content = SerializePreparedGrammar(preparedGrammar).dump(2);
	FileReader::WriteAll(filePath, content + '\n');
}

PreparedGrammar JsonPreparedGrammarStorage::ParsePreparedGrammar(const Json& source)
{
	ValidateVersion(source);
	return {
		ParseParseTable(source.at("table")),
		ParseRules(source.at("rules")),
		ParseSymbol(source.at("startSymbol")),
		ParseSymbol(source.at("eofSymbol"))
	};
}

JsonPreparedGrammarStorage::Json JsonPreparedGrammarStorage::SerializePreparedGrammar(const PreparedGrammar& grammar)
{
	return {
		{ "version", FORMAT_VERSION },
		{ "startSymbol", SerializeSymbol(grammar.startSymbol) },
		{ "eofSymbol", SerializeSymbol(grammar.eofSymbol) },
		{ "rules", SerializeRules(grammar.rules) },
		{ "table", SerializeParseTable(grammar.table) }
	};
}

void JsonPreparedGrammarStorage::ValidateVersion(const Json& source)
{
	const int version = source.at("version").get<int>();
	if (version != FORMAT_VERSION)
	{
		throw std::runtime_error(
			"Unsupported prepared grammar json version: " + std::to_string(version));
	}
}

Symbol JsonPreparedGrammarStorage::ParseSymbol(const Json& source)
{
	return {
		source.at("value").get<std::string>(),
		source.at("terminal").get<bool>()
	};
}

JsonPreparedGrammarStorage::Json JsonPreparedGrammarStorage::SerializeSymbol(const Symbol& symbol)
{
	return {
		{ "value", symbol.GetValue() },
		{ "terminal", symbol.IsTerminal() }
	};
}

std::vector<Symbol> JsonPreparedGrammarStorage::ParseSymbolArray(const Json& source)
{
	std::vector<Symbol> symbols;
	for (const Json& symbol : source)
	{
		symbols.push_back(ParseSymbol(symbol));
	}
	return symbols;
}

JsonPreparedGrammarStorage::Json JsonPreparedGrammarStorage::SerializeSymbolArray(const std::vector<Symbol>& symbols)
{
	Json result = Json::array();
	for (const Symbol& symbol : symbols)
	{
		result.push_back(SerializeSymbol(symbol));
	}
	return result;
}

SemanticTag JsonPreparedGrammarStorage::ParseSemanticTag(const Json& source)
{
	const std::string tag = source.get<std::string>();
	if (tag == "none")
	{
		return SemanticTag::NONE;
	}
	return ParseSemanticTagName(tag);
}

// TODO вынести в одно место
std::string JsonPreparedGrammarStorage::SerializeSemanticTag(const SemanticTag tag)
{
	switch (tag)
	{
	case SemanticTag::NONE: return "none";
	case SemanticTag::PASS_EXPR: return "pass_expr";
	case SemanticTag::PASS_TOKEN: return "pass_token";
	case SemanticTag::GROUP: return "group";
	case SemanticTag::UNARY: return "unary";
	case SemanticTag::BINARY: return "binary";
	case SemanticTag::BOOL_LITERAL: return "bool_literal";
	case SemanticTag::INT_LITERAL: return "int_literal";
	case SemanticTag::FLOAT_LITERAL: return "float_literal";
	case SemanticTag::STRING_LITERAL: return "string_literal";
	case SemanticTag::ARRAY_LITERAL: return "array_literal";
	case SemanticTag::IDENTIFIER: return "identifier";
	case SemanticTag::ADDRESS_OF: return "address_of";
	case SemanticTag::IDENTIFIER_LIST: return "identifier_list";
	case SemanticTag::IDENTIFIER_LIST_SINGLE: return "identifier_list_single";
	case SemanticTag::EXPRESSION_LIST: return "expression_list";
	case SemanticTag::EXPRESSION_LIST_SINGLE: return "expression_list_single";
	case SemanticTag::TYPE_NAME: return "type_name";
	case SemanticTag::ARRAY_TYPE: return "array_type";
	case SemanticTag::SLICE_TYPE: return "slice_type";
	case SemanticTag::PARAM: return "param";
	case SemanticTag::POINTER_PARAM: return "pointer_param";
	case SemanticTag::PARAM_LIST: return "param_list";
	case SemanticTag::PARAM_LIST_SINGLE: return "param_list_single";
	case SemanticTag::CALL_NO_ARGS: return "call_no_args";
	case SemanticTag::CALL: return "call";
	case SemanticTag::METHOD_CALL_NO_ARGS: return "method_call_no_args";
	case SemanticTag::METHOD_CALL: return "method_call";
	case SemanticTag::MEMBER_ACCESS: return "member_access";
	case SemanticTag::INDEX_ACCESS: return "index_access";
	case SemanticTag::ASSIGNABLE_LIST: return "assignable_list";
	case SemanticTag::ASSIGNABLE_LIST_SINGLE: return "assignable_list_single";
	case SemanticTag::ASSIGNMENT: return "assignment";
	case SemanticTag::INCREMENT: return "increment";
	case SemanticTag::DECREMENT: return "decrement";
	case SemanticTag::STRUCT_FIELD: return "struct_field";
	case SemanticTag::STRUCT_FIELD_LIST: return "struct_field_list";
	case SemanticTag::STRUCT_FIELD_LIST_SINGLE: return "struct_field_list_single";
	case SemanticTag::STRUCT_DECLARATION: return "struct_declaration";
	case SemanticTag::SHORT_VAR_DECLARATION: return "short_var_declaration";
	case SemanticTag::VAR_INFERRED_DECLARATION: return "var_inferred_declaration";
	case SemanticTag::VAR_TYPED_DECLARATION: return "var_typed_declaration";
	case SemanticTag::VAR_TYPED_INITIALIZED_DECLARATION: return "var_typed_initialized_declaration";
	case SemanticTag::EXPRESSION_STATEMENT: return "expression_statement";
	case SemanticTag::PROGRAM: return "program";
	case SemanticTag::PROGRAM_EMPTY: return "program_empty";
	case SemanticTag::STATEMENT_LIST: return "statement_list";
	case SemanticTag::STATEMENT_LIST_SINGLE: return "statement_list_single";
	case SemanticTag::IF: return "if";
	case SemanticTag::IF_ELSE: return "if_else";
	case SemanticTag::FOR_CONDITION: return "for_condition";
	case SemanticTag::FOR_CLASSIC: return "for_classic";
	case SemanticTag::BREAK: return "break";
	case SemanticTag::CONTINUE: return "continue";
	case SemanticTag::RETURN_VOID: return "return_void";
	case SemanticTag::RETURN_VALUE: return "return_value";
	case SemanticTag::FUNCTION_VOID_NO_PARAMS: return "function_void_no_params";
	case SemanticTag::FUNCTION_RETURN_NO_PARAMS: return "function_return_no_params";
	case SemanticTag::FUNCTION_VOID: return "function_void";
	case SemanticTag::FUNCTION_RETURN: return "function_return";
	case SemanticTag::METHOD_VOID_NO_PARAMS: return "method_void_no_params";
	case SemanticTag::METHOD_RETURN_NO_PARAMS: return "method_return_no_params";
	case SemanticTag::METHOD_VOID: return "method_void";
	case SemanticTag::METHOD_RETURN: return "method_return";
	case SemanticTag::BLOCK: return "block";
	case SemanticTag::BLOCK_EMPTY: return "block_empty";
	}
	return "none";
}

Action JsonPreparedGrammarStorage::ParseAction(const Json& source)
{
	return {
		ActionFormatter::ParseType(source.at("type").get<std::string>(), ActionFormat::Json),
		source.at("value").get<int>()
	};
}

JsonPreparedGrammarStorage::Json JsonPreparedGrammarStorage::SerializeAction(const Action& action)
{
	return {
		{ "type", ActionFormatter::FormatType(action.type, ActionFormat::Json) },
		{ "value", action.value }
	};
}

std::pair<Symbol, Action> JsonPreparedGrammarStorage::ParseActionEntry(const Json& source)
{
	return {
		ParseSymbol(source.at("symbol")),
		ParseAction(source.at("action"))
	};
}

JsonPreparedGrammarStorage::Json JsonPreparedGrammarStorage::SerializeActionEntry(
	const Symbol& symbol,
	const Action& action)
{
	return {
		{ "symbol", SerializeSymbol(symbol) },
		{ "action", SerializeAction(action) }
	};
}

std::map<Symbol, Action> JsonPreparedGrammarStorage::ParseActionRow(const Json& source)
{
	std::map<Symbol, Action> row;
	for (const Json& entry : source)
	{
		const auto [symbol, action] = ParseActionEntry(entry);
		row[symbol] = action;
	}
	return row;
}

JsonPreparedGrammarStorage::Json JsonPreparedGrammarStorage::SerializeActionRow(const std::map<Symbol, Action>& row)
{
	Json result = Json::array();
	for (const auto& [symbol, action] : row)
	{
		result.push_back(SerializeActionEntry(symbol, action));
	}
	return result;
}

ParserRule JsonPreparedGrammarStorage::ParseRule(const Json& source)
{
	return {
		source.at("index").get<int>(),
		ParseSymbol(source.at("lhs")),
		ParseSymbolArray(source.at("rhs")),
		ParseSemanticTag(source.at("semanticTag"))
	};
}

JsonPreparedGrammarStorage::Json JsonPreparedGrammarStorage::SerializeRule(const ParserRule& rule)
{
	return {
		{ "index", rule.index },
		{ "lhs", SerializeSymbol(rule.lhs) },
		{ "rhs", SerializeSymbolArray(rule.rhs) },
		{ "semanticTag", SerializeSemanticTag(rule.semanticTag) }
	};
}

std::vector<ParserRule> JsonPreparedGrammarStorage::ParseRules(const Json& source)
{
	std::vector<ParserRule> rules;
	for (const Json& rule : source)
	{
		rules.push_back(ParseRule(rule));
	}
	return rules;
}

JsonPreparedGrammarStorage::Json JsonPreparedGrammarStorage::SerializeRules(const std::vector<ParserRule>& rules)
{
	Json result = Json::array();
	for (const ParserRule& rule : rules)
	{
		result.push_back(SerializeRule(rule));
	}
	return result;
}

void JsonPreparedGrammarStorage::ParseTableRow(ParseTable& table, const Json& source)
{
	const int state = source.at("state").get<int>();
	table[state] = ParseActionRow(source.at("actions"));
}

JsonPreparedGrammarStorage::Json JsonPreparedGrammarStorage::SerializeTableRow(
	const int state,
	const std::map<Symbol, Action>& row)
{
	return {
		{ "state", state },
		{ "actions", SerializeActionRow(row) }
	};
}

ParseTable JsonPreparedGrammarStorage::ParseParseTable(const Json& source)
{
	ParseTable table;
	for (const Json& row : source)
	{
		ParseTableRow(table, row);
	}
	return table;
}

JsonPreparedGrammarStorage::Json JsonPreparedGrammarStorage::SerializeParseTable(const ParseTable& table)
{
	Json result = Json::array();
	for (const auto& [state, row] : table)
	{
		result.push_back(SerializeTableRow(state, row));
	}
	return result;
}
