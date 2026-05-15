#pragma once

#include "PreparedGrammarStorage.h"
#include <nlohmann/json.hpp>

class JsonPreparedGrammarStorage final : public PreparedGrammarStorage
{
public:
	[[nodiscard]] PreparedGrammar Load(const std::string& filePath) const override;
	void Save(const PreparedGrammar& preparedGrammar, const std::string& filePath) const override;

private:
	using Json = nlohmann::json;

	static constexpr int FORMAT_VERSION = 1;

	[[nodiscard]] static PreparedGrammar ParsePreparedGrammar(const Json& source);
	[[nodiscard]] static Json SerializePreparedGrammar(const PreparedGrammar& grammar);
	static void ValidateVersion(const Json& source);

	[[nodiscard]] static Symbol ParseSymbol(const Json& source);
	[[nodiscard]] static Json SerializeSymbol(const Symbol& symbol);
	[[nodiscard]] static std::vector<Symbol> ParseSymbolArray(const Json& source);
	[[nodiscard]] static Json SerializeSymbolArray(const std::vector<Symbol>& symbols);

	[[nodiscard]] static SemanticTag ParseSemanticTag(const Json& source);
	[[nodiscard]] static std::string SerializeSemanticTag(SemanticTag tag);

	[[nodiscard]] static Action ParseAction(const Json& source);
	[[nodiscard]] static Json SerializeAction(const Action& action);
	[[nodiscard]] static std::pair<Symbol, Action> ParseActionEntry(const Json& source);
	[[nodiscard]] static Json SerializeActionEntry(const Symbol& symbol, const Action& action);
	[[nodiscard]] static std::map<Symbol, Action> ParseActionRow(const Json& source);
	[[nodiscard]] static Json SerializeActionRow(const std::map<Symbol, Action>& row);

	[[nodiscard]] static ParserRule ParseRule(const Json& source);
	[[nodiscard]] static Json SerializeRule(const ParserRule& rule);
	[[nodiscard]] static std::vector<ParserRule> ParseRules(const Json& source);
	[[nodiscard]] static Json SerializeRules(const std::vector<ParserRule>& rules);

	static void ParseTableRow(ParseTable& table, const Json& source);
	[[nodiscard]] static Json SerializeTableRow(int state, const std::map<Symbol, Action>& row);
	[[nodiscard]] static ParseTable ParseParseTable(const Json& source);
	[[nodiscard]] static Json SerializeParseTable(const ParseTable& table);
};
