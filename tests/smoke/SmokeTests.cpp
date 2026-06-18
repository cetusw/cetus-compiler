#include "src/app/cli/CommandLineInterface.h"
#include "src/app/cli/commands/RunSourceDriver.h"
#include "src/app/diagnostics/CompilerError.h"
#include "src/app/diagnostics/DiagnosticStage.h"
#include "src/support/io/FileReader.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace fs = std::filesystem;

struct SmokeTestCase
{
	std::string feature;
	std::string variant;
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
	return { CETUS_SOURCE_DIR };
}

bool RequiresTests(const std::string& feature)
{
	return feature == "tests" || feature == "assertions";
}

std::string TrimTrailingWhitespace(std::string value)
{
	while (!value.empty() && (value.back() == '\n' || value.back() == '\r' || value.back() == ' ' || value.back() == '\t'))
	{
		value.pop_back();
	}

	return value;
}

std::string FormatCompilerError(const CompilerError& error)
{
	return "[ "
		+ std::string(ToString(error.GetStage()))
		+ " ] "
		+ error.what();
}

std::string RunProgram(const fs::path& sourceFile, const bool requireTests, const bool expectException)
{
	Configuration configuration;
	configuration.inputFilePath = sourceFile.string();
	configuration.requireTests = requireTests;
	configuration.report = false;
	configuration.regenerateTable = false;

	CurrentPathGuard pathGuard(GetProjectRoot());

	try
	{
		testing::internal::CaptureStdout();
		RunSourceDriver::Execute(configuration);
		return testing::internal::GetCapturedStdout();
	}
	catch (const CompilerError& error)
	{
		testing::internal::GetCapturedStdout();
		if (expectException)
		{
			return FormatCompilerError(error);
		}
		throw;
	}
	catch (const std::exception& exception)
	{
		testing::internal::GetCapturedStdout();
		if (expectException)
		{
			return "[ "
				+ std::string(ToString(DiagnosticStage::Internal))
				+ " ] "
				+ exception.what();
		}
		throw;
	}
}

void DiscoverTestsInDirectory(
	std::vector<SmokeTestCase>& testCases,
	const fs::path& featureDirectory,
	const std::string& variant)
{
	const fs::path sourceDirectory = featureDirectory / variant;
	const fs::path expectedDirectory = featureDirectory / "expected";

	if (!fs::exists(sourceDirectory) || !fs::exists(expectedDirectory))
	{
		return;
	}

	for (const auto& sourceEntry : fs::directory_iterator(sourceDirectory))
	{
		if (!sourceEntry.is_regular_file())
		{
			continue;
		}

		const fs::path& sourceFile = sourceEntry.path();
		fs::path expectedFile = expectedDirectory / sourceFile.stem();
		expectedFile.replace_extension(".txt");

		if (!fs::exists(expectedFile))
		{
			throw std::runtime_error(
				"Expected file not found: " + expectedFile.string());
		}

		const std::string feature = featureDirectory.filename().string();
		const std::string name = feature + "_" + variant + "_" + sourceFile.stem().string();
		testCases.push_back({
			feature,
			variant,
			name,
			sourceFile,
			expectedFile,
		});
	}
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
		DiscoverTestsInDirectory(testCases, featureDirectory, "positive");
		DiscoverTestsInDirectory(testCases, featureDirectory, "negative");
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
	const bool requireTests = RequiresTests(testCase.feature);
	const std::string expectedOutput = TrimTrailingWhitespace(FileReader::ReadAll(testCase.expectedFile));

	if (testCase.variant == "positive")
	{
		const std::string actualOutput = TrimTrailingWhitespace(RunProgram(testCase.sourceFile, requireTests, false));
		EXPECT_EQ(actualOutput, expectedOutput);
		return;
	}

	const std::string actualError = RunProgram(testCase.sourceFile, requireTests, true);
	EXPECT_EQ(TrimTrailingWhitespace(actualError), expectedOutput);
}

INSTANTIATE_TEST_SUITE_P(
	Smoke,
	SmokeTest,
	::testing::ValuesIn(DiscoverSmokeTests()),
	[](const auto& info) {
		return info.param.name;
	});
