#include "ActionFormatter.h"

#include <stdexcept>

namespace
{
std::string FormatCompactType(const ActionType type)
{
	switch (type)
	{
	case ActionType::SHIFT:
		return "S";
	case ActionType::REDUCE:
		return "R";
	case ActionType::GOTO:
		return "G";
	case ActionType::ACCEPT:
		return "ACC";
	case ActionType::ERROR:
		return "";
	}
	return "";
}

std::string FormatTextType(const ActionType type)
{
	switch (type)
	{
	case ActionType::SHIFT:
		return "shift";
	case ActionType::REDUCE:
		return "reduce";
	case ActionType::GOTO:
		return "goto";
	case ActionType::ACCEPT:
		return "accept";
	case ActionType::ERROR:
		return "error";
	}
	return "error";
}

bool ActionHasValue(const ActionType type)
{
	return type == ActionType::SHIFT || type == ActionType::REDUCE || type == ActionType::GOTO;
}
} // namespace

std::string ActionFormatter::Format(const Action action, const ActionFormat format)
{
	std::string type = FormatType(action.type, format);
	switch (format)
	{
	case ActionFormat::Compact:
		if (ActionHasValue(action.type))
		{
			return type + std::to_string(action.value);
		}
		return type;
	case ActionFormat::Json:
	case ActionFormat::Diagnostic:
		if (format == ActionFormat::Diagnostic && ActionHasValue(action.type))
		{
			return type + " " + std::to_string(action.value);
		}
		return type;
	}

	return "";
}

std::string ActionFormatter::FormatType(const ActionType type, const ActionFormat format)
{
	switch (format)
	{
	case ActionFormat::Compact:
		return FormatCompactType(type);
	case ActionFormat::Json:
	case ActionFormat::Diagnostic:
		return FormatTextType(type);
	}

	return "";
}

ActionType ActionFormatter::ParseType(const std::string& value, const ActionFormat format)
{
	switch (format)
	{
	case ActionFormat::Compact:
		if (value == "S")
			return ActionType::SHIFT;
		if (value == "R")
			return ActionType::REDUCE;
		if (value == "G")
			return ActionType::GOTO;
		if (value == "ACC")
			return ActionType::ACCEPT;
		if (value.empty())
			return ActionType::ERROR;
		break;
	case ActionFormat::Json:
	case ActionFormat::Diagnostic:
		if (value == "shift")
			return ActionType::SHIFT;
		if (value == "reduce")
			return ActionType::REDUCE;
		if (value == "goto")
			return ActionType::GOTO;
		if (value == "accept")
			return ActionType::ACCEPT;
		if (value == "error")
			return ActionType::ERROR;
		break;
	}

	throw std::runtime_error("Unsupported parser action type: " + value);
}
