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
	std::string feature;
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

	const fs::path smokeDirectory = GetProjectRoot() / "tests/smoke";

	for (const auto& featureEntry : fs::directory_iterator(smokeDirectory))
	{
		if (!featureEntry.is_directory())
		{
			continue;
		}

		const fs::path& featureDirectory = featureEntry.path();
		const fs::path sourceDirectory = featureDirectory / "positive";
		const fs::path expectedDirectory = featureDirectory / "expected";

		if (!fs::exists(sourceDirectory) || !fs::exists(expectedDirectory))
		{
			continue;
		}

		for (const auto& sourceEntry : fs::directory_iterator(sourceDirectory))
		{
			if (!sourceEntry.is_regular_file())
			{
				continue;
			}

			const fs::path sourceFile = sourceEntry.path();
			fs::path expectedFile = expectedDirectory / sourceFile.stem();
			expectedFile.replace_extension(".txt");

			if (!fs::exists(expectedFile))
			{
				throw std::runtime_error(
					"Expected file not found: " + expectedFile.string());
			}

			const std::string feature = featureDirectory.filename().string();

			testCases.push_back({
				feature,
				feature + "_positive_" + sourceFile.stem().string(),
				sourceFile,
				expectedFile,
			});
		}
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

	std::cout << testCase.sourceFile << std::endl;
	EXPECT_EQ(actualOutput, expectedOutput);
}

INSTANTIATE_TEST_SUITE_P(
	Smoke,
	SmokeTest,
	::testing::ValuesIn(DiscoverSmokeTests()),
	[](const auto& info) {
		return info.param.name;
	});
