#pragma once

#include <string_view>

enum class DiagnosticStage
{
	Lexical,
	Syntax,
	Semantic,
	Codegen,
	Runtime,
	Internal
};

inline std::string_view ToString(const DiagnosticStage stage)
{
	switch (stage)
	{
	case DiagnosticStage::Lexical:
		return "Lexical";
	case DiagnosticStage::Syntax:
		return "Syntax";
	case DiagnosticStage::Semantic:
		return "Semantic";
	case DiagnosticStage::Codegen:
		return "Codegen";
	case DiagnosticStage::Runtime:
		return "Runtime";
	case DiagnosticStage::Internal:
		return "Internal";
	}

	return "Internal";
}
