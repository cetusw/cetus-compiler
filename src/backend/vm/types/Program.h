#pragma once

#include "src/backend/vm/objects/ObjFunction.h"
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

struct TestDescriptor
{
	std::string name;
	std::shared_ptr<ObjFunction> function;
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
