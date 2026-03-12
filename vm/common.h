#pragma once

constexpr int FRAMES_MAX = 64;
constexpr int STACK_MAX = FRAMES_MAX * 256;
constexpr int TOTAL_POSSIBLE_OPCODES = 256;

#ifdef NDEBUG
constexpr bool DEBUG_MODE = false;
#else
constexpr bool DEBUG_MODE = true;

#define DEBUG_PRINT_CODE
#define DEBUG_TRACE_EXECUTION
#define DEBUG_STRESS_GC
#endif
