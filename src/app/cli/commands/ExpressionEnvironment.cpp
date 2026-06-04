#include "ExpressionEnvironment.h"

TypeEnvironment ExpressionEnvironment::CreateTypeEnvironment()
{
	return TypeEnvironment({
		{"a", Type::INT},
		{"b", Type::FLOAT},
		{"x", Type::INT},
		{"flag", Type::BOOL}
	});
}

RuntimeEnvironment ExpressionEnvironment::CreateRuntimeEnvironment()
{
	return RuntimeEnvironment({
		{"a", Value(10.0)},
		{"b", Value(2.5)},
		{"x", Value(7.0)},
		{"flag", Value(true)}
	});
}
