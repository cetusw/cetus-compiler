#pragma once

#include "DiagnosticStage.h"

#include <stdexcept>
#include <string>

class CompilerError final : public std::runtime_error
{
public:
	CompilerError(const DiagnosticStage stage, const std::string& message)
		: std::runtime_error(message)
		, m_stage(stage)
	{
	}

	[[nodiscard]] DiagnosticStage GetStage() const
	{
		return m_stage;
	}

private:
	DiagnosticStage m_stage;
};
