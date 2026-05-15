#pragma once

#include "types/LALR.h"
#include <string>

enum class ActionFormat
{
	Compact,
	Json
};

class ActionFormatter
{
public:
	[[nodiscard]] static std::string Format(Action action, ActionFormat format);
	[[nodiscard]] static std::string FormatType(ActionType type, ActionFormat format);
	[[nodiscard]] static ActionType ParseType(const std::string& value, ActionFormat format);
};
