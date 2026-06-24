#pragma once

#include "src/frontend/syntax/table/SemanticTag.h"
#include <string>
#include <string_view>

[[nodiscard]] SemanticTag ParseSemanticTagName(std::string_view value);
[[nodiscard]] std::string FormatSemanticTagName(SemanticTag tag);
