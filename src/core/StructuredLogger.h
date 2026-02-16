/// StructuredLogger.h — Phase 29: Structured logging with JSON output
///
/// Provides structured log entries with key-value fields suitable for
/// machine-parseable output (JSON) while maintaining backward compatibility
/// with spdlog text macros.
///
/// Pattern implemented: #26 Structured logging

#pragma once

#include "Logger.h"

#include <chrono>
#include <cstdint>
#include <initializer_list>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace markamp::core
{

/// Log severity levels matching spdlog.
enum class LogLevel : uint8_t
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
};

/// A single structured field (key-value pair).
struct LogField
{
    std::string key;
    std::string value;
};

/// A structured log entry with metadata and arbitrary fields.
struct LogEntry
{
    LogLevel level{LogLevel::Info};
    std::string message;
    std::string component;
    std::vector<LogField> fields;
    int64_t timestamp_us{0};

    /// Serialize to JSON string.
    [[nodiscard]] auto to_json() const -> std::string
    {
        std::ostringstream oss;
        oss << "{\"level\":\"" << level_name(level) << '"';
        oss << ",\"ts\":" << timestamp_us;

        if (!component.empty())
        {
            oss << ",\"component\":\"" << escape_json_string(component) << '"';
        }

        oss << ",\"msg\":\"" << escape_json_string(message) << '"';

        for (const auto& field : fields)
        {
            oss << ",\"" << escape_json_string(field.key) << "\":\""
                << escape_json_string(field.value) << '"';
        }

        oss << '}';
        return oss.str();
    }

    /// Serialize to human-readable text.
    [[nodiscard]] auto to_text() const -> std::string
    {
        std::ostringstream oss;
        oss << '[' << level_name(level) << ']';

        if (!component.empty())
        {
            oss << " [" << component << ']';
        }

        oss << ' ' << message;

        for (const auto& field : fields)
        {
            oss << ' ' << field.key << '=' << field.value;
        }

        return oss.str();
    }

private:
    [[nodiscard]] static auto level_name(LogLevel lvl) -> std::string_view
    {
        switch (lvl)
        {
            case LogLevel::Trace:
                return "TRACE";
            case LogLevel::Debug:
                return "DEBUG";
            case LogLevel::Info:
                return "INFO";
            case LogLevel::Warn:
                return "WARN";
            case LogLevel::Error:
                return "ERROR";
            case LogLevel::Fatal:
                return "FATAL";
        }
        return "UNKNOWN";
    }

    [[nodiscard]] static auto escape_json_string(std::string_view input) -> std::string
    {
        std::string output;
        output.reserve(input.size());
        for (char ch : input)
        {
            switch (ch)
            {
                case '"':
                    output += "\\\"";
                    break;
                case '\\':
                    output += "\\\\";
                    break;
                case '\n':
                    output += "\\n";
                    break;
                case '\t':
                    output += "\\t";
                    break;
                default:
                    output += ch;
            }
        }
        return output;
    }
};

/// Structured logger that builds LogEntry objects and dispatches to spdlog.
///
/// Usage:
///   StructuredLogger logger("MyComponent");
///   logger.info("User logged in", {{"user", "alice"}, {"ip", "10.0.0.1"}});
class StructuredLogger
{
public:
    enum class OutputFormat : uint8_t
    {
        Text,
        Json,
    };

    explicit StructuredLogger(std::string component, OutputFormat format = OutputFormat::Text)
        : component_(std::move(component))
        , format_(format)
    {
    }

    StructuredLogger() = default;

    void trace(std::string_view msg, std::initializer_list<LogField> fields = {})
    {
        emit(LogLevel::Trace, msg, fields);
    }

    void debug(std::string_view msg, std::initializer_list<LogField> fields = {})
    {
        emit(LogLevel::Debug, msg, fields);
    }

    void info(std::string_view msg, std::initializer_list<LogField> fields = {})
    {
        emit(LogLevel::Info, msg, fields);
    }

    void warn(std::string_view msg, std::initializer_list<LogField> fields = {})
    {
        emit(LogLevel::Warn, msg, fields);
    }

    void error(std::string_view msg, std::initializer_list<LogField> fields = {})
    {
        emit(LogLevel::Error, msg, fields);
    }

    void fatal(std::string_view msg, std::initializer_list<LogField> fields = {})
    {
        emit(LogLevel::Fatal, msg, fields);
    }

    /// Set output format.
    void set_format(OutputFormat fmt)
    {
        format_ = fmt;
    }

    /// Build a LogEntry without dispatching (for testing).
    [[nodiscard]] auto build_entry(LogLevel level_val,
                                   std::string_view msg,
                                   std::initializer_list<LogField> fields = {}) const -> LogEntry
    {
        LogEntry entry;
        entry.level = level_val;
        entry.message = std::string(msg);
        entry.component = component_;
        entry.fields = std::vector<LogField>(fields);
        entry.timestamp_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                 std::chrono::system_clock::now().time_since_epoch())
                                 .count();
        return entry;
    }

private:
    std::string component_;
    OutputFormat format_{OutputFormat::Text};

    void emit(LogLevel level_val, std::string_view msg, std::initializer_list<LogField> fields)
    {
        auto entry = build_entry(level_val, msg, fields);
        auto formatted = (format_ == OutputFormat::Json) ? entry.to_json() : entry.to_text();

        switch (level_val)
        {
            case LogLevel::Trace:
                MARKAMP_LOG_TRACE("{}", formatted);
                break;
            case LogLevel::Debug:
                MARKAMP_LOG_DEBUG("{}", formatted);
                break;
            case LogLevel::Info:
                MARKAMP_LOG_INFO("{}", formatted);
                break;
            case LogLevel::Warn:
                MARKAMP_LOG_WARN("{}", formatted);
                break;
            case LogLevel::Error:
                MARKAMP_LOG_ERROR("{}", formatted);
                break;
            case LogLevel::Fatal:
                MARKAMP_LOG_CRITICAL("{}", formatted);
                break;
        }
    }
};

/// Convenience macro for FATAL log + abort.
#define MARKAMP_LOG_FATAL(msg, ...)                                                                \
    do                                                                                             \
    {                                                                                              \
        MARKAMP_LOG_CRITICAL("FATAL: " msg, ##__VA_ARGS__);                                        \
        std::abort();                                                                              \
    } while (false)

} // namespace markamp::core
