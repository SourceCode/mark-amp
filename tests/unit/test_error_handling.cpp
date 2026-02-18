/// @file test_error_handling.cpp
/// Phase 34 Batch 34C – Task 11: Error handling tests.
/// Tests Result<T>, ErrorCode, Error struct, make_error helpers,
/// graceful degradation on invalid input.

#include "canvas/BoardSerializer.h"
#include "core/Config.h"
#include "core/Result.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::core;

// ---------------------------------------------------------------------------
// ErrorCode and SubsystemId
// ---------------------------------------------------------------------------

TEST_CASE("Error — ErrorCode names are non-empty", "[error][codes]")
{
    REQUIRE_FALSE(error_code_name(ErrorCode::Unknown).empty());
    REQUIRE_FALSE(error_code_name(ErrorCode::InvalidArgument).empty());
    REQUIRE_FALSE(error_code_name(ErrorCode::NotFound).empty());
    REQUIRE_FALSE(error_code_name(ErrorCode::IoError).empty());
    REQUIRE_FALSE(error_code_name(ErrorCode::ParseError).empty());
}

TEST_CASE("Error — SubsystemId names", "[error][subsystem]")
{
    REQUIRE_FALSE(subsystem_name(SubsystemId::Core).empty());
    REQUIRE_FALSE(subsystem_name(SubsystemId::Rendering).empty());
    REQUIRE_FALSE(subsystem_name(SubsystemId::Config).empty());
    REQUIRE_FALSE(subsystem_name(SubsystemId::Search).empty());
}

// ---------------------------------------------------------------------------
// Error struct
// ---------------------------------------------------------------------------

TEST_CASE("Error — struct fields and format", "[error][struct]")
{
    Error err;
    err.code = ErrorCode::NotFound;
    err.subsystem = SubsystemId::Workspace;
    err.message = "File not found: test.md";

    REQUIRE(err.code == ErrorCode::NotFound);
    REQUIRE(err.subsystem == SubsystemId::Workspace);

    auto formatted = err.format();
    REQUIRE_FALSE(formatted.empty());
    REQUIRE(formatted.find("File not found") != std::string::npos);
}

// ---------------------------------------------------------------------------
// make_error helpers
// ---------------------------------------------------------------------------

TEST_CASE("Error — make_error captures source location", "[error][factory]")
{
    auto err = make_error(ErrorCode::InvalidArgument, SubsystemId::Core, "bad arg");
    REQUIRE(err.code == ErrorCode::InvalidArgument);
    REQUIRE(err.subsystem == SubsystemId::Core);
    REQUIRE(err.message == "bad arg");
    // Source location should be captured
    REQUIRE_FALSE(err.file.empty());
    REQUIRE(err.line > 0);
}

TEST_CASE("Error — make_validation_error", "[error][factory]")
{
    auto err = make_validation_error("value out of range");
    REQUIRE(err.code == ErrorCode::ValidationFailed);
    REQUIRE(err.subsystem == SubsystemId::Core);
    REQUIRE(err.message == "value out of range");
}

TEST_CASE("Error — make_io_error", "[error][factory]")
{
    auto err = make_io_error("disk full");
    REQUIRE(err.code == ErrorCode::IoError);
    REQUIRE(err.subsystem == SubsystemId::Workspace);
}

TEST_CASE("Error — make_parse_error", "[error][factory]")
{
    auto err = make_parse_error("unexpected token at line 5");
    REQUIRE(err.code == ErrorCode::ParseError);
    REQUIRE(err.message.find("line 5") != std::string::npos);
}

TEST_CASE("Error — make_plugin_error", "[error][factory]")
{
    auto err = make_plugin_error("activation failed");
    REQUIRE(err.code == ErrorCode::PluginError);
}

TEST_CASE("Error — make_config_error", "[error][factory]")
{
    auto err = make_config_error("key not found");
    REQUIRE(err.code == ErrorCode::ConfigError);
    REQUIRE(err.subsystem == SubsystemId::Config);
}

TEST_CASE("Error — make_render_error", "[error][factory]")
{
    auto err = make_render_error("GPU context lost");
    REQUIRE(err.code == ErrorCode::RenderError);
    REQUIRE(err.subsystem == SubsystemId::Rendering);
}

// ---------------------------------------------------------------------------
// Result<T> usage
// ---------------------------------------------------------------------------

TEST_CASE("Error — Result success value", "[error][result]")
{
    Result<int> result = 42;
    REQUIRE(result.has_value());
    REQUIRE(result.value() == 42);
}

TEST_CASE("Error — Result error value", "[error][result]")
{
    Result<int> result = std::unexpected(make_validation_error("bad value"));
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::ValidationFailed);
}

TEST_CASE("Error — Result string success", "[error][result]")
{
    Result<std::string> result = std::string("hello");
    REQUIRE(result.has_value());
    REQUIRE(result.value() == "hello");
}

// ---------------------------------------------------------------------------
// Graceful degradation tests
// ---------------------------------------------------------------------------

TEST_CASE("Error — BoardSerializer rejects corrupt JSON", "[error][degradation]")
{
    markamp::canvas::BoardSerializer serializer;
    // validate_json returns false for corrupt input
    REQUIRE_FALSE(serializer.validate_json("{definitely not valid json!!! }}}"));
    REQUIRE_FALSE(serializer.validate_json("not json at all"));
}

TEST_CASE("Error — Config returns default for missing key", "[error][degradation]")
{
    Config config;
    // Non-existent keys return defaults
    REQUIRE(config.get_int("nonexistent.key", 42) == 42);
    REQUIRE(config.get_string("nonexistent.key", "fallback") == "fallback");
    REQUIRE(config.get_bool("nonexistent.key", true) == true);
    REQUIRE(config.get_double("nonexistent.key", 3.14) == 3.14);
}

TEST_CASE("Error — Config type coercion is transparent", "[error][degradation]")
{
    Config config;
    // Config transparently converts between types
    config.set("test.value", 100);
    // Config will coerce int 100 to string "100"
    REQUIRE(config.get_string("test.value", "default") == "100");
    REQUIRE(config.get_int("test.value") == 100);
}
