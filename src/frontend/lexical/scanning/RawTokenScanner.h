#pragma once

#include "CommentSkipper.h"
#include "LiteralScanner.h"
#include "OperatorScanner.h"
#include "src/frontend/lexical/core/LexicalDiagnostic.h"
#include "src/frontend/lexical/core/SourceCursor.h"

#include <optional>
#include <vector>

struct RawScanResult
{
	std::vector<Token> tokens;
	std::optional<LexicalDiagnostic> error;
};

class RawTokenScanner
{
public:
	explicit RawTokenScanner(SourceCursor& cursor);

	[[nodiscard]] RawScanResult Scan() const;

private:
	void ScanToken(std::vector<Token>& tokens, std::optional<LexicalDiagnostic>& error) const;
	void AddNewline(std::vector<Token>& tokens) const;
	[[nodiscard]] static bool IsWhitespace(char c);
	[[nodiscard]] static bool IsIdentifierStart(char c);
	[[nodiscard]] static bool IsDigit(char c);

	SourceCursor& m_cursor;
	LiteralScanner m_literalScanner;
	CommentSkipper m_commentSkipper;
	OperatorScanner m_operatorScanner;
};
