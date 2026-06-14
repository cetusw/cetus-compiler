#pragma once

#include "src/backend/vm/objects/ObjFunction.h"
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

enum class PropertyGeneratorKind
{
	INT,
	BOOL,
	STRING
};

struct PropertyParameterDescriptor
{
	std::string name;
	PropertyGeneratorKind generator = PropertyGeneratorKind::INT;
};

struct PropertyDescriptor
{
	std::shared_ptr<ObjFunction> function;
	std::vector<PropertyParameterDescriptor> parameters;
};

struct TestDescriptor
{
	std::string name;
	std::shared_ptr<ObjFunction> function;
	std::vector<PropertyDescriptor> properties;
};

struct TestManifest
{
	std::vector<TestDescriptor> tests;
	std::unordered_set<std::string> coveredSymbols;
};

struct Program
{
	std::vector<std::shared_ptr<ObjFunction>> functions;
	std::shared_ptr<ObjFunction> entryPoint;
	TestManifest testManifest;
};
