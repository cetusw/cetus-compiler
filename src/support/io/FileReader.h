#pragma once

#include <string>
#include <vector>

class FileReader
{
public:
	static std::string ReadAll(const std::string& path);
	static std::vector<std::string> ReadLines(const std::string& path);
	static void WriteAll(const std::string& path, const std::string& content);
};
