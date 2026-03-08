// test_api_version.cpp — 10 tests for ApiVersion
#include "core/ApiVersion.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ApiVersion default is 0.0.0", "[api_version]")
{
    ApiVersion v;
    CHECK(v.major == 0);
    CHECK(v.minor == 0);
    CHECK(v.patch == 0);
}

TEST_CASE("ApiVersion to_string formats correctly", "[api_version]")
{
    ApiVersion v{1, 2, 3};
    CHECK(v.to_string() == "1.2.3");
}

TEST_CASE("ApiVersion parse valid string", "[api_version]")
{
    auto result = ApiVersion::parse("2.5.10");
    REQUIRE(result.has_value());
    CHECK(result->major == 2);
    CHECK(result->minor == 5);
    CHECK(result->patch == 10);
}

TEST_CASE("ApiVersion parse invalid returns error", "[api_version]")
{
    auto result = ApiVersion::parse("not_a_version");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("ApiVersion comparison operators", "[api_version]")
{
    ApiVersion v1{1, 0, 0};
    ApiVersion v2{1, 1, 0};
    ApiVersion v3{2, 0, 0};
    CHECK(v1 < v2);
    CHECK(v2 < v3);
    CHECK(v1 == v1);
    CHECK_FALSE(v1 == v2);
}

TEST_CASE("ApiVersion is_compatible_with same major", "[api_version]")
{
    ApiVersion host{1, 5, 0};
    ApiVersion required{1, 3, 0};
    CHECK(host.is_compatible_with(required));
}

TEST_CASE("ApiVersion is_compatible_with rejects different major", "[api_version]")
{
    ApiVersion host{2, 0, 0};
    ApiVersion required{1, 0, 0};
    CHECK_FALSE(host.is_compatible_with(required));
}

TEST_CASE("ApiVersion is_compatible_with rejects too new required", "[api_version]")
{
    ApiVersion host{1, 2, 0};
    ApiVersion required{1, 5, 0};
    CHECK_FALSE(host.is_compatible_with(required));
}

TEST_CASE("ApiVersion current returns non-zero version", "[api_version]")
{
    auto current = ApiVersion::current();
    // At least major or minor should be > 0
    CHECK((current.major > 0 || current.minor > 0));
}

TEST_CASE("ApiVersion compare returns correct ordering", "[api_version]")
{
    ApiVersion v1{1, 0, 0};
    ApiVersion v2{1, 1, 0};
    CHECK(v1.compare(v2) < 0);
    CHECK(v2.compare(v1) > 0);
    CHECK(v1.compare(v1) == 0);
}
