/// @file test_v25_p14_extension_scope.cpp
/// @brief V25 Phase 14: Extension scope matrix tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P14: Supported contributions", "[v25][p14]")
{
    ExtensionScopeMatrix matrix;
    ContributionPointEntry p;
    p.point_id = "commands";
    p.support = ContributionSupport::kSupported;
    matrix.add_point(p);
    REQUIRE(matrix.is_supported("commands"));
    REQUIRE(matrix.supported_points().size() == 1);
}

TEST_CASE("V25 P14: Unsupported contributions", "[v25][p14]")
{
    ExtensionScopeMatrix matrix;
    ContributionPointEntry p;
    p.point_id = "debuggers";
    p.support = ContributionSupport::kUnsupported;
    matrix.add_point(p);
    REQUIRE_FALSE(matrix.is_supported("debuggers"));
    REQUIRE(matrix.unsupported_points().size() == 1);
}

TEST_CASE("V25 P14: Point count", "[v25][p14]")
{
    ExtensionScopeMatrix matrix;
    REQUIRE(matrix.point_count() == 0);
    ContributionPointEntry p; p.point_id = "test";
    matrix.add_point(p);
    REQUIRE(matrix.point_count() == 1);
}
