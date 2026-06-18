#pragma once

#include "src/frontend/lexical/core/LexicalDiagnostic.h"
#include "src/frontend/lexical/core/SourceCursor.h"
#include "src/frontend/lexical/core/TokenFactory.h"

#include <optional>

struct TokenScanResult
{
	std::optional<Token> token;
	std::optional<LexicalDiagnostic> error;
};

class LiteralScanner
{
public:
	explicit LiteralScanner(SourceCursor& cursor);

	[[nodiscard]] Token ScanIdentifierOrKeyword() const;
	[[nodiscard]] Token ScanNumber() const;
	[[nodiscard]] TokenScanResult ScanString(char quoteType) const;

private:
	[[nodiscard]] static bool IsIdentifierPart(char c);
	[[nodiscard]] static bool IsDigit(char c);

	SourceCursor& m_cursor;
};
