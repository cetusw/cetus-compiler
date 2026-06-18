#pragma once

#include <string>

struct LexicalDiagnostic
{
	std::string message;
	int line = 1;
};
