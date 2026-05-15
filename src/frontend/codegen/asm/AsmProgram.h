#pragma once

#include <string>
#include <vector>

class AsmProgram
{
public:
	void AddRoData(std::string line);
	void AddData(std::string line);
	void AddText(std::string line);

	[[nodiscard]] std::string Build() const;

private:
	[[nodiscard]] static std::string BuildSection(const std::string& header, const std::vector<std::string>& lines);

	std::vector<std::string> m_rodata;
	std::vector<std::string> m_data;
	std::vector<std::string> m_text;
};
