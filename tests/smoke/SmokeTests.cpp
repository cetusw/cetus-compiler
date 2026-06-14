#include "src/app/cli/CommandLineInterface.h"
#include "src/app/cli/commands/RunSourceDriver.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace fs = std::filesystem;

struct SmokeTestCase
{
	std::string name;
	fs::path sourceFile;
	fs::path expectedFile;
};

class CurrentPathGuard
{
public:
	explicit CurrentPathGuard(const fs::path& newCurrentPath)
		: oldCurrentPath(fs::current_path())
	{
		fs::current_path(newCurrentPath);
	}

	~CurrentPathGuard()
	{
		fs::current_path(oldCurrentPath);
	}

private:
	fs::path oldCurrentPath;
};

fs::path GetProjectRoot()
{
	return fs::path(CETUS_SOURCE_DIR);
}

std::string ReadFile(const fs::path& filePath)
{
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		throw std::runtime_error(
			"Failed to open file: " + filePath.string());
	}

	return {
		std::istreambuf_iterator(file),
		std::istreambuf_iterator<char>()
	};
}

std::string RunProgram(const fs::path& sourceFile)
{
	Configuration configuration;
	configuration.inputFilePath = sourceFile.string();
	configuration.requireTests = false;
	configuration.report = false;
	configuration.regenerateTable = false;

	RunSourceDriver driver;

	CurrentPathGuard pathGuard(GetProjectRoot());

	testing::internal::CaptureStdout();

	driver.Execute(configuration);

	return testing::internal::GetCapturedStdout();
}

std::vector<SmokeTestCase> DiscoverSmokeTests()
{
	std::vector<SmokeTestCase> testCases;

	const fs::path sourceDirectory = GetProjectRoot() / "tests/smoke/source";
	const fs::path expectedDirectory = GetProjectRoot() / "tests/smoke/expected";

	for (const auto& entry : fs::directory_iterator(sourceDirectory))
	{
		if (!entry.is_regular_file())
		{
			continue;
		}

		const fs::path sourceFile = entry.path();

		const fs::path expectedFile = expectedDirectory / sourceFile.stem().replace_extension(".cetus");

		if (!fs::exists(expectedFile))
		{
			throw std::runtime_error(
				"Expected file not found: " + expectedFile.string());
		}

		testCases.push_back({
			sourceFile.stem().string(),
			sourceFile,
			expectedFile,
		});
	}

	std::ranges::sort(testCases,
		[](const SmokeTestCase& left, const SmokeTestCase& right) {
			return left.name < right.name;
		});

	return testCases;
}

class SmokeTest
	: public ::testing::TestWithParam<SmokeTestCase>
{
};

TEST_P(SmokeTest, MatchesExpectedOutput)
{
	const SmokeTestCase& testCase = GetParam();

	const std::string actualOutput = RunProgram(testCase.sourceFile);

	const std::string expectedOutput = ReadFile(testCase.expectedFile);

	EXPECT_EQ(actualOutput, expectedOutput);
}

INSTANTIATE_TEST_SUITE_P(
	Smoke,
	SmokeTest,
	::testing::ValuesIn(DiscoverSmokeTests()),
	[](const auto& info) {
		return info.param.name;
	});
