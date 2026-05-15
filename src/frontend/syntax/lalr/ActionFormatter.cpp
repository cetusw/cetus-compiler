#include "ActionFormatter.h"

#include <stdexcept>

std::string ActionFormatter::Format(const Action action, const ActionFormat format)
{
	switch (format)
	{
	case ActionFormat::Compact:
		switch (action.type)
		{
		case ActionType::SHIFT: return "S" + std::to_string(action.value);
		case ActionType::REDUCE: return "R" + std::to_string(action.value);
		case ActionType::GOTO: return "G" + std::to_string(action.value);
		case ActionType::ACCEPT: return "ACC";
		case ActionType::ERROR: return "";
		}
		break;
	case ActionFormat::Json:
		return FormatType(action.type, format);
	}

	return "";
}

std::string ActionFormatter::FormatType(const ActionType type, const ActionFormat format)
{
	// TODO сделать один формат?
	switch (format)
	{
	case ActionFormat::Compact:
		switch (type)
		{
		case ActionType::SHIFT: return "S";
		case ActionType::REDUCE: return "R";
		case ActionType::GOTO: return "G";
		case ActionType::ACCEPT: return "ACC";
		case ActionType::ERROR: return "";
		}
		break;
	case ActionFormat::Json:
		switch (type)
		{
		case ActionType::SHIFT: return "shift";
		case ActionType::REDUCE: return "reduce";
		case ActionType::GOTO: return "goto";
		case ActionType::ACCEPT: return "accept";
		case ActionType::ERROR: return "error";
		}
		break;
	}

	return "";
}

ActionType ActionFormatter::ParseType(const std::string& value, const ActionFormat format)
{
	switch (format)
	{
	case ActionFormat::Compact:
		if (value == "S") return ActionType::SHIFT;
		if (value == "R") return ActionType::REDUCE;
		if (value == "G") return ActionType::GOTO;
		if (value == "ACC") return ActionType::ACCEPT;
		if (value.empty()) return ActionType::ERROR;
		break;
	case ActionFormat::Json:
		if (value == "shift") return ActionType::SHIFT;
		if (value == "reduce") return ActionType::REDUCE;
		if (value == "goto") return ActionType::GOTO;
		if (value == "accept") return ActionType::ACCEPT;
		if (value == "error") return ActionType::ERROR;
		break;
	}

	throw std::runtime_error("Unsupported parser action type: " + value);
}
