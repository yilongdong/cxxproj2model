#pragma once
#include <spdlog/spdlog.h>

// PS: AI生成注释真厉害
// A macro that takes in a variable number of arguments and passes them to spdlog::info.
#define LOG_INFO(...) spdlog::info(__VA_ARGS__);
// A macro that takes in a variable number of arguments and passes them to spdlog::error.
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__);
