#pragma once

#include <string>

class Symbol
{
public:
	Symbol(std::string value, bool isTerminal);
	Symbol();

	bool operator<(const Symbol& other) const;
	bool operator==(const Symbol& other) const;
	bool operator!=(const Symbol& other) const;

	[[nodiscard]] std::string GetValue() const;
	[[nodiscard]] bool IsTerminal() const;
	[[nodiscard]] bool IsEpsilon() const;

private:
	std::string m_value{};
	bool m_isTerminal{};
};
