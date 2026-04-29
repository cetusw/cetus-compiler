#pragma once

#include <string>
#include <vector>

struct ParseResult
{
	bool success = false;
	int line = 1;
	std::string message;
	std::vector<std::string> expectedTerminals;
};
