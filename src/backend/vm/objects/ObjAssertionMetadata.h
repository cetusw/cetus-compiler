#pragma once

#include "Obj.h"
#include <optional>
#include <string>

struct AssertionDescriptor
{
	int sourceLine = 0;
	std::string sourceText;
	std::optional<std::string> message;
};

class ObjAssertionMetadata final : public Obj
{
public:
	explicit ObjAssertionMetadata(AssertionDescriptor descriptor);

	[[nodiscard]] const AssertionDescriptor& GetDescriptor() const;

private:
	AssertionDescriptor m_descriptor;
};
