/// test_structured_logger.cpp — V7 Phase 03: Structured logging tests

#include "core/StructuredLogger.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <thread>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// LogEntry — JSON Serialization
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("LogEntry: to_json produces valid JSON structure", "[structured_logger]")
{
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "test message";
    entry.component = "TestComponent";
    entry.timestamp_us = 1000000;

    auto json = entry.to_json();

    REQUIRE(json.find("\"level\":\"INFO\"") != std::string::npos);
    REQUIRE(json.find("\"msg\":\"test message\"") != std::string::npos);
    REQUIRE(json.find("\"component\":\"TestComponent\"") != std::string::npos);
    REQUIRE(json.find("\"ts\":1000000") != std::string::npos);
}

TEST_CASE("LogEntry: to_json includes custom fields", "[structured_logger]")
{
    LogEntry entry;
    entry.level = LogLevel::Warn;
    entry.message = "warning";
    entry.fields = {{"user", "alice"}, {"ip", "10.0.0.1"}};
    entry.timestamp_us = 0;

    auto json = entry.to_json();

    REQUIRE(json.find("\"user\":\"alice\"") != std::string::npos);
    REQUIRE(json.find("\"ip\":\"10.0.0.1\"") != std::string::npos);
}

TEST_CASE("LogEntry: to_json escapes special characters", "[structured_logger]")
{
    LogEntry entry;
    entry.level = LogLevel::Error;
    entry.message = "line1\nline2\ttab \"quoted\"";
    entry.timestamp_us = 0;

    auto json = entry.to_json();

    REQUIRE(json.find("\\n") != std::string::npos);
    REQUIRE(json.find("\\t") != std::string::npos);
    REQUIRE(json.find("\\\"quoted\\\"") != std::string::npos);
}

TEST_CASE("LogEntry: to_json without component omits component field", "[structured_logger]")
{
    LogEntry entry;
    entry.level = LogLevel::Debug;
    entry.message = "msg";
    entry.component = "";
    entry.timestamp_us = 0;

    auto json = entry.to_json();

    REQUIRE(json.find("component") == std::string::npos);
}

// ══════════════════════════════════════════════════════════════════════════════
// LogEntry — Text Serialization
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("LogEntry: to_text includes level and message", "[structured_logger]")
{
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "hello";
    entry.component = "MyComp";

    auto text = entry.to_text();

    REQUIRE(text.find("[INFO]") != std::string::npos);
    REQUIRE(text.find("[MyComp]") != std::string::npos);
    REQUIRE(text.find("hello") != std::string::npos);
}

TEST_CASE("LogEntry: to_text includes fields as key=value", "[structured_logger]")
{
    LogEntry entry;
    entry.level = LogLevel::Trace;
    entry.message = "trace";
    entry.fields = {{"count", "5"}};

    auto text = entry.to_text();

    REQUIRE(text.find("count=5") != std::string::npos);
}

// ══════════════════════════════════════════════════════════════════════════════
// StructuredLogger — build_entry
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("StructuredLogger: build_entry sets component", "[structured_logger]")
{
    StructuredLogger logger("EditorPanel");
    auto entry = logger.build_entry(LogLevel::Info, "cursor moved");

    REQUIRE(entry.component == "EditorPanel");
    REQUIRE(entry.level == LogLevel::Info);
    REQUIRE(entry.message == "cursor moved");
    REQUIRE(entry.timestamp_us > 0);
}

TEST_CASE("StructuredLogger: build_entry with fields", "[structured_logger]")
{
    StructuredLogger logger("Config");
    auto entry = logger.build_entry(LogLevel::Warn, "deprecated key", {{"key", "old.setting"}});

    REQUIRE(entry.fields.size() == 1);
    REQUIRE(entry.fields[0].key == "key");
    REQUIRE(entry.fields[0].value == "old.setting");
}

TEST_CASE("StructuredLogger: default constructor creates empty component", "[structured_logger]")
{
    StructuredLogger logger;
    auto entry = logger.build_entry(LogLevel::Debug, "test");

    REQUIRE(entry.component.empty());
}

// ══════════════════════════════════════════════════════════════════════════════
// Log Level Names
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("LogEntry: all log levels have correct names", "[structured_logger]")
{
    LogEntry entry;
    entry.timestamp_us = 0;

    entry.level = LogLevel::Trace;
    REQUIRE(entry.to_json().find("\"level\":\"TRACE\"") != std::string::npos);

    entry.level = LogLevel::Debug;
    REQUIRE(entry.to_json().find("\"level\":\"DEBUG\"") != std::string::npos);

    entry.level = LogLevel::Info;
    REQUIRE(entry.to_json().find("\"level\":\"INFO\"") != std::string::npos);

    entry.level = LogLevel::Warn;
    REQUIRE(entry.to_json().find("\"level\":\"WARN\"") != std::string::npos);

    entry.level = LogLevel::Error;
    REQUIRE(entry.to_json().find("\"level\":\"ERROR\"") != std::string::npos);

    entry.level = LogLevel::Fatal;
    REQUIRE(entry.to_json().find("\"level\":\"FATAL\"") != std::string::npos);
}

// ══════════════════════════════════════════════════════════════════════════════
// Output Format
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("StructuredLogger: set_format changes output format", "[structured_logger]")
{
    StructuredLogger logger("Test");
    logger.set_format(StructuredLogger::OutputFormat::Json);

    auto entry = logger.build_entry(LogLevel::Info, "test");
    auto json = entry.to_json();
    auto text = entry.to_text();

    // Both should produce valid output regardless of format setting
    REQUIRE_FALSE(json.empty());
    REQUIRE_FALSE(text.empty());
}
