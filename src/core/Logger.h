#pragma once

#include <spdlog/spdlog.h>
#include <string>

namespace markamp::core
{

void initLogger();
auto getLogFilePath() -> std::string;

} // namespace markamp::core

// Convenience logging macros
#define MARKAMP_LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define MARKAMP_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define MARKAMP_LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define MARKAMP_LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define MARKAMP_LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define MARKAMP_LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)
