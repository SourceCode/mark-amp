/// test_result_types.cpp — V7 Phase 01: Result<T> and error taxonomy tests

#include "core/Result.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// Result<T> — Construction
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("Result: success construction with value", "[result]")
{
    Result<int> result = 42;
    REQUIRE(result.has_value());
    REQUIRE(*result == 42);
}

TEST_CASE("Result: success construction with void", "[result]")
{
    Result<void> result{};
    REQUIRE(result.has_value());
}

TEST_CASE("Result: error construction", "[result]")
{
    Result<int> result =
        std::unexpected(make_error(ErrorCode::InvalidArgument, SubsystemId::Core, "test error"));
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidArgument);
    REQUIRE(result.error().subsystem == SubsystemId::Core);
    REQUIRE(result.error().message == "test error");
}

TEST_CASE("Result: error with void type", "[result]")
{
    Result<void> result = std::unexpected(make_validation_error("bad input"));
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::ValidationFailed);
}

TEST_CASE("Result: string result success", "[result]")
{
    Result<std::string> result = std::string("hello");
    REQUIRE(result.has_value());
    REQUIRE(*result == "hello");
}

// ══════════════════════════════════════════════════════════════════════════════
// Error — Factory Functions
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("make_error creates structured error", "[result]")
{
    auto err = make_error(ErrorCode::FileNotFound, SubsystemId::Workspace, "no file");
    REQUIRE(err.code == ErrorCode::FileNotFound);
    REQUIRE(err.subsystem == SubsystemId::Workspace);
    REQUIRE(err.message == "no file");
    REQUIRE_FALSE(err.file.empty()); // source_location captures file
    REQUIRE(err.line > 0);
}

TEST_CASE("make_validation_error defaults to Core subsystem", "[result]")
{
    auto err = make_validation_error("bad value");
    REQUIRE(err.code == ErrorCode::ValidationFailed);
    REQUIRE(err.subsystem == SubsystemId::Core);
    REQUIRE(err.message == "bad value");
}

TEST_CASE("make_io_error defaults to Workspace subsystem", "[result]")
{
    auto err = make_io_error("disk full");
    REQUIRE(err.code == ErrorCode::IoError);
    REQUIRE(err.subsystem == SubsystemId::Workspace);
}

TEST_CASE("make_parse_error works", "[result]")
{
    auto err = make_parse_error("unexpected token", ErrorCode::JsonParseError);
    REQUIRE(err.code == ErrorCode::JsonParseError);
    REQUIRE(err.message == "unexpected token");
}

TEST_CASE("make_plugin_error defaults to ExtensionHost", "[result]")
{
    auto err = make_plugin_error("activate failed", ErrorCode::PluginActivateFailed);
    REQUIRE(err.code == ErrorCode::PluginActivateFailed);
    REQUIRE(err.subsystem == SubsystemId::ExtensionHost);
}

TEST_CASE("make_config_error defaults to Config subsystem", "[result]")
{
    auto err = make_config_error("missing key");
    REQUIRE(err.code == ErrorCode::ConfigError);
    REQUIRE(err.subsystem == SubsystemId::Config);
}

TEST_CASE("make_render_error defaults to Rendering subsystem", "[result]")
{
    auto err = make_render_error("token missing", ErrorCode::ThemeTokenMissing);
    REQUIRE(err.code == ErrorCode::ThemeTokenMissing);
    REQUIRE(err.subsystem == SubsystemId::Rendering);
}

// ══════════════════════════════════════════════════════════════════════════════
// Error — Formatting
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("Error::format includes subsystem, code, and message", "[result]")
{
    Error err{ErrorCode::FileNotFound, SubsystemId::Workspace, "config.json", "main.cpp", 42};
    auto formatted = err.format();

    REQUIRE(formatted.find("Workspace") != std::string::npos);
    REQUIRE(formatted.find("FileNotFound") != std::string::npos);
    REQUIRE(formatted.find("config.json") != std::string::npos);
    REQUIRE(formatted.find("main.cpp") != std::string::npos);
    REQUIRE(formatted.find("42") != std::string::npos);
}

TEST_CASE("Error::format without file info", "[result]")
{
    Error err{ErrorCode::Unknown, SubsystemId::Core, "oops", "", 0};
    auto formatted = err.format();

    REQUIRE(formatted.find("Core") != std::string::npos);
    REQUIRE(formatted.find("Unknown") != std::string::npos);
    REQUIRE(formatted.find("oops") != std::string::npos);
    // Should NOT contain source location parens
    REQUIRE(formatted.find("(") == std::string::npos);
}

// ══════════════════════════════════════════════════════════════════════════════
// Name Helpers
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("subsystem_name returns correct strings", "[result]")
{
    REQUIRE(subsystem_name(SubsystemId::Core) == "Core");
    REQUIRE(subsystem_name(SubsystemId::Rendering) == "Rendering");
    REQUIRE(subsystem_name(SubsystemId::ExtensionHost) == "ExtensionHost");
    REQUIRE(subsystem_name(SubsystemId::Config) == "Config");
}

TEST_CASE("error_code_name returns correct strings", "[result]")
{
    REQUIRE(error_code_name(ErrorCode::FileNotFound) == "FileNotFound");
    REQUIRE(error_code_name(ErrorCode::ValidationFailed) == "ValidationFailed");
    REQUIRE(error_code_name(ErrorCode::PluginQuarantined) == "PluginQuarantined");
    REQUIRE(error_code_name(ErrorCode::IntegerOverflow) == "IntegerOverflow");
}

// ══════════════════════════════════════════════════════════════════════════════
// MARKAMP_GUARD Macro
// ══════════════════════════════════════════════════════════════════════════════

static auto guarded_function(int input) -> Result<int>
{
    MARKAMP_GUARD(input > 0,
                  make_validation_error("input must be positive", ErrorCode::InvalidRange));
    return input * 2;
}

TEST_CASE("MARKAMP_GUARD passes on true condition", "[result]")
{
    auto result = guarded_function(5);
    REQUIRE(result.has_value());
    REQUIRE(*result == 10);
}

TEST_CASE("MARKAMP_GUARD returns error on false condition", "[result]")
{
    auto result = guarded_function(-1);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidRange);
}

// ══════════════════════════════════════════════════════════════════════════════
// MARKAMP_TRY Macro
// ══════════════════════════════════════════════════════════════════════════════

static auto inner_function(bool succeed) -> Result<void>
{
    if (!succeed)
    {
        return std::unexpected(
            make_error(ErrorCode::IoError, SubsystemId::Workspace, "disk error"));
    }
    return {};
}

static auto outer_function(bool succeed) -> Result<int>
{
    MARKAMP_TRY(inner_function(succeed));
    return 42;
}

TEST_CASE("MARKAMP_TRY passes through on success", "[result]")
{
    auto result = outer_function(true);
    REQUIRE(result.has_value());
    REQUIRE(*result == 42);
}

TEST_CASE("MARKAMP_TRY propagates error on failure", "[result]")
{
    auto result = outer_function(false);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::IoError);
    REQUIRE(result.error().subsystem == SubsystemId::Workspace);
}
