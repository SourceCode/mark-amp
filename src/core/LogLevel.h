#pragma once

#include <cstdint>
#include <cstring>

namespace markamp::core
{

/// Log severity levels shared across output panel, problems panel, and debug console.
/// Ordered by severity so comparison operators can be used for filtering.
enum class LogLevel : uint8_t
{
    kTrace = 0,
    kDebug = 1,
    kInfo = 2,
    kWarning = 3,
    kError = 4,
    kFatal = 5,
};

/// Convert a LogLevel to a human-readable string.
[[nodiscard]] constexpr auto log_level_name(LogLevel level) -> const char*
{
    switch (level)
    {
        case LogLevel::kTrace:
            return "TRACE";
        case LogLevel::kDebug:
            return "DEBUG";
        case LogLevel::kInfo:
            return "INFO";
        case LogLevel::kWarning:
            return "WARN";
        case LogLevel::kError:
            return "ERROR";
        case LogLevel::kFatal:
            return "FATAL";
    }
    return "UNKNOWN";
}

/// Parse a severity string to a LogLevel enum.
/// Returns kInfo if the string is not recognized.
[[nodiscard]] inline auto log_level_from_string(const char* str) -> LogLevel
{
    if (str == nullptr)
    {
        return LogLevel::kInfo;
    }
    if (std::strcmp(str, "TRACE") == 0)
    {
        return LogLevel::kTrace;
    }
    if (std::strcmp(str, "DEBUG") == 0)
    {
        return LogLevel::kDebug;
    }
    if (std::strcmp(str, "INFO") == 0)
    {
        return LogLevel::kInfo;
    }
    if (std::strcmp(str, "WARN") == 0 || std::strcmp(str, "WARNING") == 0)
    {
        return LogLevel::kWarning;
    }
    if (std::strcmp(str, "ERROR") == 0)
    {
        return LogLevel::kError;
    }
    if (std::strcmp(str, "FATAL") == 0)
    {
        return LogLevel::kFatal;
    }
    return LogLevel::kInfo;
}

/// Severity comparison — allows filtering like `level >= kWarning`.
[[nodiscard]] constexpr auto operator<=(LogLevel lhs, LogLevel rhs) -> bool
{
    return static_cast<uint8_t>(lhs) <= static_cast<uint8_t>(rhs);
}

[[nodiscard]] constexpr auto operator>=(LogLevel lhs, LogLevel rhs) -> bool
{
    return static_cast<uint8_t>(lhs) >= static_cast<uint8_t>(rhs);
}

[[nodiscard]] constexpr auto operator<(LogLevel lhs, LogLevel rhs) -> bool
{
    return static_cast<uint8_t>(lhs) < static_cast<uint8_t>(rhs);
}

[[nodiscard]] constexpr auto operator>(LogLevel lhs, LogLevel rhs) -> bool
{
    return static_cast<uint8_t>(lhs) > static_cast<uint8_t>(rhs);
}

/// Count of log levels for iteration.
constexpr int kLogLevelCount = 6;

} // namespace markamp::core
