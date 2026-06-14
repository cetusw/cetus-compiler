#include "PropertyGenerator.h"

#include "src/backend/vm/objects/ObjString.h"

#include <string>

std::optional<PropertyGeneratorKind> PropertyGenerator::ResolveKind(const TypeDescriptor& type)
{
	if (type == Type::INT)
	{
		return PropertyGeneratorKind::INT;
	}
	if (type == Type::BOOL)
	{
		return PropertyGeneratorKind::BOOL;
	}
	if (type == Type::STRING)
	{
		return PropertyGeneratorKind::STRING;
	}
	return std::nullopt;
}

bool PropertyGenerator::Supports(const TypeDescriptor& type)
{
	return ResolveKind(type).has_value();
}

Value PropertyGenerator::Generate(const PropertyGeneratorKind kind, std::mt19937_64& generator)
{
	switch (kind)
	{
	case PropertyGeneratorKind::INT:
	{
		std::uniform_int_distribution<RuntimeInt> distribution(-100, 100);
		return Value(distribution(generator));
	}
	case PropertyGeneratorKind::BOOL:
	{
		std::bernoulli_distribution distribution(0.5);
		return Value(distribution(generator));
	}
	case PropertyGeneratorKind::STRING:
	{
		std::uniform_int_distribution lengthDistribution(0, 8);
		std::uniform_int_distribution charDistribution(0, 25);
		const int length = lengthDistribution(generator);
		std::string value;
		value.reserve(static_cast<std::size_t>(length));
		for (int index = 0; index < length; ++index)
		{
			value.push_back(static_cast<char>('a' + charDistribution(generator)));
		}
		return Value(std::make_shared<ObjString>(value));
	}
	}

	return Value();
}
