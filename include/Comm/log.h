#pragma once
#include <cstdio>

// PS: AI生成注释真厉害
// A macro that takes in a variable number of arguments and passes them to spdlog::info.
#define LOG_INFO(format, ...) fprintf(stderr, format "\n", ##__VA_ARGS__);
// A macro that takes in a variable number of arguments and passes them to spdlog::error.
#define LOG_ERROR(format, ...) fprintf(stderr, format "\n", ##__VA_ARGS__);
