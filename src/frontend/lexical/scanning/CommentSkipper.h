#pragma once

#include "src/frontend/lexical/core/LexicalDiagnostic.h"
#include "src/frontend/lexical/core/SourceCursor.h"

#include <optional>

class CommentSkipper
{
public:
	explicit CommentSkipper(SourceCursor& cursor);

	[[nodiscard]] bool TrySkipComment(std::optional<LexicalDiagnostic>& error) const;

private:
	void SkipSingleLineComment() const;
	[[nodiscard]] bool SkipMultiLineComment(std::optional<LexicalDiagnostic>& error) const;

	SourceCursor& m_cursor;
};
