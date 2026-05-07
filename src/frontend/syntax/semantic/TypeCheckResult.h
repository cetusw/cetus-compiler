#pragma once

#include "Type.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

class Expr;

struct TypeCheckResult
{
	using NodeTypes = std::unordered_map<const Expr*, Type>;

	[[nodiscard]] static TypeCheckResult Success(Type rootType, NodeTypes nodeTypes)
	{
		TypeCheckResult result;
		result.type = rootType;
		result.nodeTypes = std::move(nodeTypes);
		return result;
	}

	[[nodiscard]] static TypeCheckResult Error(std::string message, NodeTypes nodeTypes = {})
	{
		TypeCheckResult result;
		result.error = std::move(message);
		result.nodeTypes = std::move(nodeTypes);
		return result;
	}

	[[nodiscard]] std::optional<Type> GetNodeType(const Expr& expr) const
	{
		const auto it = nodeTypes.find(&expr);
		if (it == nodeTypes.end())
		{
			return std::nullopt;
		}

		return it->second;
	}

	Type type = Type::ERROR;
	std::optional<std::string> error;
	NodeTypes nodeTypes;
};
