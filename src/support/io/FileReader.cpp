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
