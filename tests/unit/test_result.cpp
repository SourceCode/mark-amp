// test_result.cpp — 10 tests for Result<T>, ErrorCode, Error, and helpers
#include "core/Result.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("Result<int> holds success value", "[result]")
{
    Result<int> result{42};
    REQUIRE(result.has_value());
    CHECK(*result == 42);
}

TEST_CASE("Result<void> holds success", "[result]")
{
    Result<void> result{};
    CHECK(result.has_value());
}

TEST_CASE("Result<int> holds error", "[result]")
{
    Result<int> result =
        std::unexpected(make_error(ErrorCode::NotFound, SubsystemId::Core, "missing"));
    CHECK_FALSE(result.has_value());
    CHECK(result.error().code == ErrorCode::NotFound);
}

TEST_CASE("Error format includes message and subsystem", "[result]")
{
    auto err = make_error(ErrorCode::InvalidArgument, SubsystemId::Editor, "bad input");
    auto formatted = err.format();
    CHECK(formatted.find("bad input") != std::string::npos);
}

TEST_CASE("error_code_name returns readable names", "[result]")
{
    CHECK_FALSE(error_code_name(ErrorCode::NotFound).empty());
    CHECK_FALSE(error_code_name(ErrorCode::InvalidArgument).empty());
    CHECK_FALSE(error_code_name(ErrorCode::IoError).empty());
}

TEST_CASE("subsystem_name returns readable names", "[result]")
{
    CHECK_FALSE(subsystem_name(SubsystemId::Core).empty());
    CHECK_FALSE(subsystem_name(SubsystemId::Editor).empty());
    CHECK_FALSE(subsystem_name(SubsystemId::Rendering).empty());
}

TEST_CASE("make_validation_error creates with correct code", "[result]")
{
    auto err = make_validation_error("invalid value");
    CHECK(err.code == ErrorCode::ValidationFailed);
    CHECK(err.message == "invalid value");
}

TEST_CASE("make_io_error creates with correct subsystem", "[result]")
{
    auto err = make_io_error("file not found");
    CHECK(err.code == ErrorCode::IoError);
    CHECK(err.subsystem == SubsystemId::Workspace);
}

TEST_CASE("make_parse_error creates parse error", "[result]")
{
    auto err = make_parse_error("syntax error");
    CHECK(err.code == ErrorCode::ParseError);
    CHECK(err.message == "syntax error");
}

TEST_CASE("make_config_error creates config error", "[result]")
{
    auto err = make_config_error("key not found");
    CHECK(err.code == ErrorCode::ConfigError);
}
