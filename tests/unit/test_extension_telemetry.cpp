/// test_extension_telemetry.cpp — Comprehensive tests for ExtensionTelemetry
#include "core/ExtensionTelemetry.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("ExtensionTelemetry: record_activation creates entry", "[extension_telemetry][positive]")
{
    ExtensionTelemetry tel;
    tel.record_activation("ext.a", std::chrono::milliseconds(150));
    auto* data = tel.get_telemetry("ext.a");
    REQUIRE(data != nullptr);
    REQUIRE(data->activation_time == std::chrono::milliseconds(150));
}

TEST_CASE("ExtensionTelemetry: record_api_call increments count", "[extension_telemetry][positive]")
{
    ExtensionTelemetry tel;
    tel.record_activation("ext.a", std::chrono::milliseconds(10));
    tel.record_api_call("ext.a");
    tel.record_api_call("ext.a");
    tel.record_api_call("ext.a");
    auto* data = tel.get_telemetry("ext.a");
    REQUIRE(data != nullptr);
    REQUIRE(data->api_call_count == 3);
}

TEST_CASE("ExtensionTelemetry: record_error increments count", "[extension_telemetry][positive]")
{
    ExtensionTelemetry tel;
    tel.record_activation("ext.a", std::chrono::milliseconds(10));
    tel.record_error("ext.a");
    auto* data = tel.get_telemetry("ext.a");
    REQUIRE(data != nullptr);
    REQUIRE(data->error_count == 1);
}

TEST_CASE("ExtensionTelemetry: record_command increments count", "[extension_telemetry][positive]")
{
    ExtensionTelemetry tel;
    tel.record_activation("ext.a", std::chrono::milliseconds(10));
    tel.record_command("ext.a");
    tel.record_command("ext.a");
    auto* data = tel.get_telemetry("ext.a");
    REQUIRE(data != nullptr);
    REQUIRE(data->command_execution_count == 2);
}

TEST_CASE("ExtensionTelemetry: tracked_extensions lists all", "[extension_telemetry][positive]")
{
    ExtensionTelemetry tel;
    tel.record_activation("ext.a", std::chrono::milliseconds(10));
    tel.record_activation("ext.b", std::chrono::milliseconds(20));
    auto exts = tel.tracked_extensions();
    REQUIRE(exts.size() == 2);
}

TEST_CASE("ExtensionTelemetry: clear removes one extension", "[extension_telemetry][positive]")
{
    ExtensionTelemetry tel;
    tel.record_activation("ext.a", std::chrono::milliseconds(10));
    tel.record_activation("ext.b", std::chrono::milliseconds(20));
    tel.clear("ext.a");
    REQUIRE(tel.get_telemetry("ext.a") == nullptr);
    REQUIRE(tel.get_telemetry("ext.b") != nullptr);
}

TEST_CASE("ExtensionTelemetry: clear_all removes everything", "[extension_telemetry][positive]")
{
    ExtensionTelemetry tel;
    tel.record_activation("ext.a", std::chrono::milliseconds(10));
    tel.record_activation("ext.b", std::chrono::milliseconds(20));
    tel.clear_all();
    REQUIRE(tel.tracked_extensions().empty());
}

// ── Negative Tests ──

TEST_CASE("ExtensionTelemetry: get_telemetry for unknown returns nullptr",
          "[extension_telemetry][negative]")
{
    ExtensionTelemetry tel;
    REQUIRE(tel.get_telemetry("nonexistent") == nullptr);
}

TEST_CASE("ExtensionTelemetry: tracked_extensions empty initially",
          "[extension_telemetry][negative]")
{
    ExtensionTelemetry tel;
    REQUIRE(tel.tracked_extensions().empty());
}

TEST_CASE("ExtensionTelemetry: clear on unknown extension does not crash",
          "[extension_telemetry][negative]")
{
    ExtensionTelemetry tel;
    tel.clear("nonexistent"); // Should not crash or throw
    REQUIRE(tel.tracked_extensions().empty());
}

// ── Edge Cases ──

TEST_CASE("ExtensionTelemetry: zero activation time is valid", "[extension_telemetry][edge]")
{
    ExtensionTelemetry tel;
    tel.record_activation("ext.a", std::chrono::milliseconds(0));
    auto* data = tel.get_telemetry("ext.a");
    REQUIRE(data != nullptr);
    REQUIRE(data->activation_time == std::chrono::milliseconds(0));
}

TEST_CASE("ExtensionTelemetry: multiple clear_all calls are safe", "[extension_telemetry][edge]")
{
    ExtensionTelemetry tel;
    tel.clear_all();
    tel.clear_all();
    REQUIRE(tel.tracked_extensions().empty());
}

TEST_CASE("ExtensionTelemetry: re-record after clear creates fresh entry",
          "[extension_telemetry][edge]")
{
    ExtensionTelemetry tel;
    tel.record_activation("ext.a", std::chrono::milliseconds(100));
    tel.record_api_call("ext.a");
    tel.clear("ext.a");
    tel.record_activation("ext.a", std::chrono::milliseconds(50));
    auto* data = tel.get_telemetry("ext.a");
    REQUIRE(data != nullptr);
    REQUIRE(data->activation_time == std::chrono::milliseconds(50));
    REQUIRE(data->api_call_count == 0);
}
