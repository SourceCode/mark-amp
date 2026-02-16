/// test_structured_logging.cpp — Phase 29: StructuredLogger tests

#include "core/StructuredLogger.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::core;

TEST_CASE("LogEntry: to_json produces valid JSON", "[structured_logging]")
{
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "test message";
    entry.component = "TestComponent";
    entry.timestamp_us = 1234567890;
    entry.fields = {{"key1", "value1"}, {"key2", "value2"}};

    auto json = entry.to_json();

    REQUIRE(json.find("\"level\":\"INFO\"") != std::string::npos);
    REQUIRE(json.find("\"msg\":\"test message\"") != std::string::npos);
    REQUIRE(json.find("\"component\":\"TestComponent\"") != std::string::npos);
    REQUIRE(json.find("\"key1\":\"value1\"") != std::string::npos);
    REQUIRE(json.find("\"key2\":\"value2\"") != std::string::npos);
}

TEST_CASE("LogEntry: to_text produces readable text", "[structured_logging]")
{
    LogEntry entry;
    entry.level = LogLevel::Warn;
    entry.message = "warning!";
    entry.component = "Engine";
    entry.fields = {{"file", "main.cpp"}};

    auto text = entry.to_text();

    REQUIRE(text.find("[WARN]") != std::string::npos);
    REQUIRE(text.find("[Engine]") != std::string::npos);
    REQUIRE(text.find("warning!") != std::string::npos);
    REQUIRE(text.find("file=main.cpp") != std::string::npos);
}

TEST_CASE("LogEntry: escapes JSON special characters", "[structured_logging]")
{
    LogEntry entry;
    entry.level = LogLevel::Error;
    entry.message = "line1\nline2\ttab\"quote";
    entry.timestamp_us = 0;

    auto json = entry.to_json();

    REQUIRE(json.find("\\n") != std::string::npos);
    REQUIRE(json.find("\\t") != std::string::npos);
    REQUIRE(json.find("\\\"") != std::string::npos);
}

TEST_CASE("LogEntry: all log levels have names", "[structured_logging]")
{
    LogEntry entry;
    entry.timestamp_us = 0;
    entry.message = "test";

    entry.level = LogLevel::Trace;
    REQUIRE(entry.to_json().find("TRACE") != std::string::npos);

    entry.level = LogLevel::Debug;
    REQUIRE(entry.to_json().find("DEBUG") != std::string::npos);

    entry.level = LogLevel::Info;
    REQUIRE(entry.to_json().find("INFO") != std::string::npos);

    entry.level = LogLevel::Warn;
    REQUIRE(entry.to_json().find("WARN") != std::string::npos);

    entry.level = LogLevel::Error;
    REQUIRE(entry.to_json().find("ERROR") != std::string::npos);

    entry.level = LogLevel::Fatal;
    REQUIRE(entry.to_json().find("FATAL") != std::string::npos);
}

TEST_CASE("StructuredLogger: build_entry populates fields", "[structured_logging]")
{
    StructuredLogger logger("MyApp");

    auto entry = logger.build_entry(LogLevel::Info, "started", {{"port", "8080"}});

    REQUIRE(entry.message == "started");
    REQUIRE(entry.component == "MyApp");
    REQUIRE(entry.level == LogLevel::Info);
    REQUIRE(entry.fields.size() == 1);
    REQUIRE(entry.fields[0].key == "port");
    REQUIRE(entry.fields[0].value == "8080");
    REQUIRE(entry.timestamp_us > 0);
}

TEST_CASE("StructuredLogger: empty component", "[structured_logging]")
{
    StructuredLogger logger;

    auto entry = logger.build_entry(LogLevel::Debug, "no component");
    auto json = entry.to_json();

    // No component field in JSON
    REQUIRE(json.find("\"component\"") == std::string::npos);
}

TEST_CASE("LogEntry: empty fields list", "[structured_logging]")
{
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "simple";
    entry.timestamp_us = 100;

    auto json = entry.to_json();
    REQUIRE(json.find("\"msg\":\"simple\"") != std::string::npos);
}
