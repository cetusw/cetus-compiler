#pragma once

#include "src/frontend/lexical/core/LexicalDiagnostic.h"
#include "src/frontend/lexical/core/SourceCursor.h"
#include "src/frontend/lexical/core/TokenFactory.h"

#include <optional>

class OperatorScanner
{
public:
	explicit OperatorScanner(SourceCursor& cursor);

	[[nodiscard]] std::optional<Token> TryScan(char current) const;
	[[nodiscard]] std::optional<LexicalDiagnostic> GetError() const;

private:
	[[nodiscard]] Token MatchToken(char expected, TokenType ifMatch, TokenType ifNoMatch) const;
	[[nodiscard]] Token ScanLess() const;
	[[nodiscard]] Token ScanGreater() const;
	[[nodiscard]] std::optional<Token> ScanDot() const;
	[[nodiscard]] Token ScanAmpersand() const;
	[[nodiscard]] std::optional<Token> ScanQuestion() const;

	SourceCursor& m_cursor;
	mutable std::optional<LexicalDiagnostic> m_error;
};
