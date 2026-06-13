#pragma once

#include "src/backend/vm/testing/TestRunResult.h"
#include <iosfwd>

class TestReporter
{
public:
	static void Print(const TestRunResult& result, std::ostream& output);
};
