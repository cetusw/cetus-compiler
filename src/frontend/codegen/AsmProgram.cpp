#include "AsmProgram.h"

#include <sstream>
#include <utility>

void AsmProgram::AddRoData(std::string line)
{
	m_rodata.push_back(std::move(line));
}

void AsmProgram::AddData(std::string line)
{
	m_data.push_back(std::move(line));
}

void AsmProgram::AddText(std::string line)
{
	m_text.push_back(std::move(line));
}

std::string AsmProgram::Build() const
{
	std::ostringstream output;
	output << ".intel_syntax noprefix\n\n";

	const std::string rodataSection = BuildSection(".section .rodata", m_rodata);
	if (!rodataSection.empty())
	{
		output << rodataSection << "\n\n";
	}

	const std::string dataSection = BuildSection(".section .data", m_data);
	if (!dataSection.empty())
	{
		output << dataSection << "\n\n";
	}

	const std::string textSection = BuildSection(".section .text", m_text);
	if (!textSection.empty())
	{
		output << textSection << '\n';
	}

	return output.str();
}

std::string AsmProgram::BuildSection(const std::string& header, const std::vector<std::string>& lines)
{
	if (lines.empty())
	{
		return {};
	}

	std::ostringstream output;
	output << header << '\n';
	for (size_t i = 0; i < lines.size(); ++i)
	{
		output << lines[i];
		if (i + 1 < lines.size())
		{
			output << '\n';
		}
	}

	return output.str();
}
