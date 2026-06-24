#include <gtest/gtest.h>

#include "src/frontend/lexical/core/Token.h"
#include "src/frontend/parsergen/grammar/Grammar.h"
#include "src/frontend/parsergen/grammar/GrammarLoader.h"
#include "src/frontend/parsergen/grammar/GrammarSpecialSymbols.h"
#include "src/frontend/parsergen/grammar/SemanticTagFormatter.h"
#include "src/frontend/parsergen/grammar/sets/GrammarSetsBuilder.h"
#include "src/frontend/parsergen/grammar/sets/resolvers/FirstSetResolver.h"
#include "src/frontend/parsergen/lalr/ActionFormatter.h"
#include "src/frontend/parsergen/lalr/LALRBuilder.h"
#include "src/frontend/parsergen/storage/JsonPreparedGrammarStorage.h"
#include "src/frontend/syntax/SyntaxAnalyzer.h"
#include "src/frontend/syntax/parser/TokenSymbolMapper.h"

#include <array>
#include <filesystem>
#include <fstream>

namespace
{
Grammar BuildMinimalGrammar()
{
	Grammar grammar;
	grammar.AddRule(Symbol("Program", false), { Symbol("INT_LIT", true) });
	grammar.SetStartSymbol(Symbol("Program", false));
	return grammar;
}
} // namespace

TEST(GrammarLoaderTests, LoadFromLinesParsesAlternativesAndStartSymbol)
{
	const Grammar grammar = GrammarLoader::LoadFromLines({ "~Program~ -> ~Expr~",
		"~Expr~ -> INT_LIT | FLOAT_LIT" });

	EXPECT_EQ(grammar.GetStartSymbol(), Symbol("Program", false));
	const std::vector<Production>& rules = grammar.GetRules();
	ASSERT_EQ(rules.size(), 3U);
	EXPECT_EQ(rules[0].GetLhs(), Symbol("Program", false));
	EXPECT_EQ(rules[1].GetLhs(), Symbol("Expr", false));
	EXPECT_EQ(rules[2].GetLhs(), Symbol("Expr", false));
	EXPECT_EQ(rules[1].GetRhs().front(), Symbol("INT_LIT", true));
	EXPECT_EQ(rules[2].GetRhs().front(), Symbol("FLOAT_LIT", true));
}

TEST(GrammarLoaderTests, LoadFromLinesRejectsInvalidArrow)
{
	EXPECT_THROW(GrammarLoader::LoadFromLines({ "~Program~ => INT_LIT" }), std::runtime_error);
}

TEST(GrammarLoaderTests, LoadFromLinesRejectsInvalidSemanticTag)
{
	EXPECT_THROW(GrammarLoader::LoadFromLines({ "~Program~ -> INT_LIT @unknown_tag" }), std::runtime_error);
}

TEST(GrammarLoaderTests, LoadFromLinesRejectsTerminalLhs)
{
	EXPECT_THROW(GrammarLoader::LoadFromLines({ "Program -> INT_LIT" }), std::runtime_error);
}

TEST(GrammarLoaderTests, LoadFromLinesRejectsMultipleSemanticTags)
{
	EXPECT_THROW(GrammarLoader::LoadFromLines({ "~Program~ -> INT_LIT @pass_token @group" }), std::runtime_error);
}

TEST(GrammarLoaderTests, LoadFromLinesRejectsTaggedAlternativesOnOneLine)
{
	EXPECT_THROW(GrammarLoader::LoadFromLines({ "~Program~ -> INT_LIT | FLOAT_LIT @pass_token" }), std::runtime_error);
}

TEST(SemanticTagFormatterTests, FormatsAndParsesRoundTrip)
{
	EXPECT_EQ(FormatSemanticTagName(SemanticTag::PASS_TOKEN), "pass_token");
	EXPECT_EQ(ParseSemanticTagName("pass_token"), SemanticTag::PASS_TOKEN);
	EXPECT_EQ(FormatSemanticTagName(SemanticTag::NONE), "none");
}

TEST(ActionFormatterTests, JsonFormatRoundTripsAllActionTypes)
{
	constexpr std::array actionTypes = {
		ActionType::SHIFT,
		ActionType::REDUCE,
		ActionType::GOTO,
		ActionType::ACCEPT,
		ActionType::ERROR
	};

	for (const ActionType actionType : actionTypes)
	{
		const std::string serialized = ActionFormatter::FormatType(actionType, ActionFormat::Json);
		EXPECT_EQ(ActionFormatter::ParseType(serialized, ActionFormat::Json), actionType);
	}
}

TEST(GrammarSetsBuilderTests, CalculatesNullableFirstAndFollow)
{
	Grammar grammar;
	grammar.AddRule(Symbol("Program", false), { Symbol("Expr", false) });
	grammar.AddRule(Symbol("Expr", false), { Symbol("Term", false), Symbol("ExprTail", false) });
	grammar.AddRule(Symbol("ExprTail", false), { Symbol("PLUS", true), Symbol("Term", false), Symbol("ExprTail", false) });
	grammar.AddRule(Symbol("ExprTail", false), {});
	grammar.AddRule(Symbol("Term", false), { Symbol("INT_LIT", true) });
	grammar.SetStartSymbol(Symbol("Program", false));

	const GrammarSets sets = GrammarSetsBuilder::Build(grammar);

	EXPECT_TRUE(sets.nullable.contains(Symbol("ExprTail", false)));
	EXPECT_FALSE(sets.nullable.contains(Symbol("Term", false)));
	EXPECT_TRUE(sets.first.at(Symbol("Term", false)).contains(Symbol("INT_LIT", true)));
	EXPECT_TRUE(sets.first.at(Symbol("ExprTail", false)).contains(EpsilonSymbol()));
	EXPECT_TRUE(sets.follow.at(Symbol("Program", false)).contains(EofSymbol()));
	EXPECT_TRUE(sets.follow.at(Symbol("Term", false)).contains(Symbol("PLUS", true)));
	EXPECT_TRUE(sets.follow.at(Symbol("Term", false)).contains(EofSymbol()));
}

TEST(FirstSetResolverTests, ResolvesSequenceFirstSet)
{
	NullableSet nullable;
	nullable.insert(Symbol("A", false));
	FirstSets firstSets;
	firstSets[Symbol("A", false)] = { Symbol("a", true), EpsilonSymbol() };
	firstSets[Symbol("B", false)] = { Symbol("b", true) };

	const FirstSetResolver resolver(nullable, firstSets);
	const std::array sequence = { Symbol("A", false), Symbol("B", false) };
	const std::set<Symbol> first = resolver.GetFirstOfSequence(sequence);

	EXPECT_TRUE(first.contains(Symbol("a", true)));
	EXPECT_TRUE(first.contains(Symbol("b", true)));
	EXPECT_FALSE(first.contains(EpsilonSymbol()));
}

TEST(JsonPreparedGrammarStorageTests, LoadRejectsUnsupportedVersion)
{
	const std::filesystem::path tempFile = std::filesystem::temp_directory_path() / "cetus_invalid_prepared_grammar.json";
	std::filesystem::remove(tempFile);
	const JsonPreparedGrammarStorage storage;

	EXPECT_NO_THROW(std::filesystem::create_directories(tempFile.parent_path()));
	std::ofstream output(tempFile);
	ASSERT_TRUE(output.is_open());
	output << R"({"version":999})";
	output.close();

	EXPECT_THROW(static_cast<void>(storage.Load(tempFile.string())), std::runtime_error);

	std::filesystem::remove(tempFile);
}

TEST(TokenSymbolMapperTests, RejectsNewlineToken)
{
	const Token token{ TokenType::NEWLINE, "\\n", 3 };
	EXPECT_THROW(TokenSymbolMapper::MapTokenToGrammarSymbol(token), std::runtime_error);
}

TEST(SyntaxAnalyzerTests, RejectsEmptyTokenStream)
{
	const SyntaxAnalyzer analyzer(PreparedGrammar{});
	const ParseResult result = analyzer.Analyze({});

	EXPECT_FALSE(result.success);
	EXPECT_EQ(result.line, 1);
	EXPECT_EQ(result.message, "Token stream is empty.");
}

TEST(SyntaxAnalyzerTests, ReportsUnexpectedTokenWithExpectedTerminals)
{
	PreparedGrammar grammar;
	grammar.table[0][Symbol("INT_LIT", true)] = Action{ ActionType::SHIFT, 1 };
	const SyntaxAnalyzer analyzer(std::move(grammar));

	const ParseResult result = analyzer.Analyze({ Token{ TokenType::FLOAT_LIT, "1.5", 7 } });

	EXPECT_FALSE(result.success);
	EXPECT_EQ(result.line, 7);
	EXPECT_EQ(result.expectedTerminals, std::vector<std::string>{ "INT_LIT" });
	EXPECT_EQ(result.message, "Syntax error at line 7: unexpected token '1.5'");
}

TEST(SyntaxAnalyzerTests, ReportsUnexpectedEndOfStream)
{
	PreparedGrammar grammar;
	grammar.table[0][Symbol("INT_LIT", true)] = Action{ ActionType::SHIFT, 1 };
	const SyntaxAnalyzer analyzer(std::move(grammar));

	const ParseResult result = analyzer.Analyze({ Token{ TokenType::INT_LIT, "1", 3 } });

	EXPECT_FALSE(result.success);
	EXPECT_EQ(result.line, 3);
	EXPECT_EQ(result.message, "Unexpected end of token stream.");
}

TEST(LALRBuilderTests, BuildProducesShiftReduceAndAcceptActions)
{
	LALRBuilder builder(BuildMinimalGrammar());
	const PreparedGrammar prepared = builder.Build();

	bool hasShift = false;
	bool hasReduce = false;
	bool hasAccept = false;
	for (const auto& [state, row] : prepared.table)
	{
		(void)state;
		for (const auto& [symbol, action] : row)
		{
			if (symbol == Symbol("INT_LIT", true) && action.type == ActionType::SHIFT)
			{
				hasShift = true;
			}
			if (action.type == ActionType::REDUCE && action.value == 1)
			{
				hasReduce = true;
			}
			if (symbol == prepared.eofSymbol && action.type == ActionType::ACCEPT)
			{
				hasAccept = true;
			}
		}
	}

	EXPECT_TRUE(hasShift);
	EXPECT_TRUE(hasReduce);
	EXPECT_TRUE(hasAccept);
}
