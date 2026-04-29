#pragma once

#include <string>

class FileReader
{
public:
	static std::string ReadAll(const std::string& path);
};
