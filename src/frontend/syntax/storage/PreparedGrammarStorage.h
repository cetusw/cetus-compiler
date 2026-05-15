#pragma once

#include "src/frontend/syntax/lalr/types/PreparedGrammar.h"
#include <string>

class PreparedGrammarStorage
{
public:
	virtual ~PreparedGrammarStorage() = default;

	[[nodiscard]] virtual PreparedGrammar Load(const std::string& filePath) const = 0;
	virtual void Save(const PreparedGrammar& preparedGrammar, const std::string& filePath) const = 0;
};
