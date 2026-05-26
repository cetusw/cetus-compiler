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
	case SemanticTag::IDENTIFIER: return "identifier";
	case SemanticTag::MEMBER_ACCESS: return "member_access";
	case SemanticTag::INDEX_ACCESS: return "index_access";
	case SemanticTag::ASSIGNMENT: return "assignment";
	case SemanticTag::SEQUENCE: return "sequence";
	case SemanticTag::IF: return "if";
	case SemanticTag::IF_ELSE: return "if_else";
	case SemanticTag::PRINTF: return "printf";
	case SemanticTag::BLOCK: return "block";
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
