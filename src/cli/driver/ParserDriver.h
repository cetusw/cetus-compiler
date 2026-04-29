#pragma once

#include "src/cli/CommandLineInterface.h"
#include "src/common/grammar/Grammar.h"
#include "src/common/types/Token.h"

#include <bits/stl_vector.h>

class ParserDriver
{
public:
	static void Execute(const Configuration& configuration);

private:
	static void CheckIfError(const std::vector<Token>& tokens);
	static Grammar GetGrammar();
};
