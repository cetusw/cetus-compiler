#include "LexicalAnalyzer.h"

#include "core/SourceCursor.h"
#include "core/TokenFactory.h"
#include "pipeline/SemicolonInserter.h"
#include "pipeline/TokenPostProcessor.h"
#include "scanning/RawTokenScanner.h"

#include <utility>

LexicalAnalyzer::LexicalAnalyzer(std::string source)
	: m_source(std::move(source))
{
}

LexerResult LexicalAnalyzer::ScanTokens() const
{
	SourceCursor cursor(m_source);
	RawTokenScanner scanner(cursor);
	const RawScanResult rawResult = scanner.Scan();

	if (rawResult.error.has_value())
	{
		return {
			{},
			rawResult.error->message,
			rawResult.error->line
		};
	}

	std::vector<Token> tokens = SemicolonInserter::Insert(rawResult.tokens);
	TokenPostProcessor::Process(tokens);
	tokens.push_back(TokenFactory::Make(TokenType::EOF_TOKEN, "", cursor.GetLine()));

	return {
		std::move(tokens),
		std::nullopt,
		cursor.GetLine()
	};
}
