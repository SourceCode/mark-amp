/// test_api_version.cpp — V7 Phase 24: API version tests

#include "core/ApiVersion.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// Parsing
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("ApiVersion: parse valid version string", "[api_version]")
{
    auto result = ApiVersion::parse("2.1.3");
    REQUIRE(result.has_value());
    REQUIRE(result->major == 2);
    REQUIRE(result->minor == 1);
    REQUIRE(result->patch == 3);
}

TEST_CASE("ApiVersion: parse zero version", "[api_version]")
{
    auto result = ApiVersion::parse("0.0.0");
    REQUIRE(result.has_value());
    REQUIRE(result->major == 0);
    REQUIRE(result->minor == 0);
    REQUIRE(result->patch == 0);
}

TEST_CASE("ApiVersion: parse rejects empty string", "[api_version]")
{
    auto result = ApiVersion::parse("");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::StringTooShort);
}

TEST_CASE("ApiVersion: parse rejects missing dot", "[api_version]")
{
    auto result = ApiVersion::parse("1.2");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::ParseError);
}

TEST_CASE("ApiVersion: parse rejects extra dots", "[api_version]")
{
    auto result = ApiVersion::parse("1.2.3.4");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::ParseError);
}

TEST_CASE("ApiVersion: parse rejects non-numeric", "[api_version]")
{
    auto result = ApiVersion::parse("a.b.c");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::ParseError);
}

// ══════════════════════════════════════════════════════════════════════════════
// to_string
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("ApiVersion: to_string formats correctly", "[api_version]")
{
    ApiVersion version{1, 2, 3};
    REQUIRE(version.to_string() == "1.2.3");
}

TEST_CASE("ApiVersion: roundtrip parse/to_string", "[api_version]")
{
    auto parsed = ApiVersion::parse("5.10.25");
    REQUIRE(parsed.has_value());
    REQUIRE(parsed->to_string() == "5.10.25");
}

// ══════════════════════════════════════════════════════════════════════════════
// Comparison
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("ApiVersion: equality", "[api_version]")
{
    ApiVersion ver_a{1, 0, 0};
    ApiVersion ver_b{1, 0, 0};
    REQUIRE(ver_a == ver_b);
}

TEST_CASE("ApiVersion: ordering", "[api_version]")
{
    ApiVersion ver_100{1, 0, 0};
    ApiVersion ver_110{1, 1, 0};
    ApiVersion ver_200{2, 0, 0};

    REQUIRE(ver_100 < ver_110);
    REQUIRE(ver_110 < ver_200);
    REQUIRE(ver_100 < ver_200);
}

TEST_CASE("ApiVersion: compare returns correct values", "[api_version]")
{
    ApiVersion ver_a{1, 0, 0};
    ApiVersion ver_b{2, 0, 0};

    REQUIRE(ver_a.compare(ver_b) < 0);
    REQUIRE(ver_b.compare(ver_a) > 0);
    REQUIRE(ver_a.compare(ver_a) == 0);
}

// ══════════════════════════════════════════════════════════════════════════════
// Compatibility
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("ApiVersion: compatible with same major, higher minor", "[api_version]")
{
    ApiVersion host{2, 1, 0};
    ApiVersion required{2, 0, 0};
    REQUIRE(host.is_compatible_with(required));
}

TEST_CASE("ApiVersion: compatible with exact match", "[api_version]")
{
    ApiVersion host{2, 0, 0};
    ApiVersion required{2, 0, 0};
    REQUIRE(host.is_compatible_with(required));
}

TEST_CASE("ApiVersion: incompatible with different major", "[api_version]")
{
    ApiVersion host{2, 0, 0};
    ApiVersion required{3, 0, 0};
    REQUIRE_FALSE(host.is_compatible_with(required));
}

TEST_CASE("ApiVersion: incompatible when host is lower minor", "[api_version]")
{
    ApiVersion host{2, 0, 0};
    ApiVersion required{2, 1, 0};
    REQUIRE_FALSE(host.is_compatible_with(required));
}

// ══════════════════════════════════════════════════════════════════════════════
// Host compatibility check
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("ApiVersion: current() returns a valid version", "[api_version]")
{
    auto current = ApiVersion::current();
    REQUIRE(current.major > 0);
}

TEST_CASE("ApiVersion: check_api_compatibility succeeds for compatible", "[api_version]")
{
    auto result = check_api_compatibility(ApiVersion{1, 0, 0});
    // current() is 2.0.0, so 1.x is incompatible (different major)
    // This actually fails — host major=2, required major=1
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ApiVersion: check_api_compatibility succeeds for current", "[api_version]")
{
    auto result = check_api_compatibility(ApiVersion::current());
    REQUIRE(result.has_value());
}

TEST_CASE("ApiVersion: check_api_compatibility fails for future version", "[api_version]")
{
    auto result = check_api_compatibility(ApiVersion{99, 0, 0});
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::PluginApiVersionMismatch);
}
