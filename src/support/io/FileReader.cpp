#include "FileReader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

std::string FileReader::ReadAll(const std::string& path)
{
	std::ifstream input(path);
	if (!input.is_open())
	{
		throw std::runtime_error("Failed to open file: " + path);
	}

	std::ostringstream content;
	content << input.rdbuf();
	return content.str();
}

std::vector<std::string> FileReader::ReadLines(const std::string& path)
{
	std::ifstream input(path);
	if (!input.is_open())
	{
		throw std::runtime_error("Failed to open file: " + path);
	}

	std::vector<std::string> lines;
	std::string line;
	while (std::getline(input, line))
	{
		lines.push_back(line);
	}
	return lines;
}

void FileReader::WriteAll(const std::string& path, const std::string& content)
{
	std::ofstream output(path, std::ios::binary | std::ios::trunc);
	if (!output)
	{
		throw std::runtime_error("Failed to open file for writing: " + path);
	}

	output << content;
	if (!output)
	{
		throw std::runtime_error("Failed to write file: " + path);
	}
}
