#pragma once

enum class ActionType
{
	SHIFT,
	REDUCE,
	GOTO,
	ACCEPT,
	ERROR
};

struct Action
{
	ActionType type = ActionType::ERROR;
	int value = -1;
};