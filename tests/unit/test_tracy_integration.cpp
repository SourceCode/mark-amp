#include "core/TracyIntegration.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

// ── TracyIntegration: Zero-Cost Macro Compilation ──
// These tests verify that all Tracy macros compile correctly when Tracy
// is disabled (the default debug build). When MARKAMP_TRACY_ENABLED is
// not defined, all macros must expand to no-ops without compilation errors.

TEST_CASE("TracyIntegration: zone macro compiles to no-op", "[tracy]")
{
    MARKAMP_TRACY_ZONE("test_zone");
    // If we reach here, the macro compiled successfully as a no-op
    REQUIRE(true);
}

TEST_CASE("TracyIntegration: zone color macro compiles to no-op", "[tracy]")
{
    MARKAMP_TRACY_ZONE_COLOR("test_zone_color", 0xFF0000);
    REQUIRE(true);
}

TEST_CASE("TracyIntegration: frame mark compiles to no-op", "[tracy]")
{
    MARKAMP_TRACY_FRAME_MARK;
    REQUIRE(true);
}

TEST_CASE("TracyIntegration: plot macro compiles to no-op", "[tracy]")
{
    MARKAMP_TRACY_PLOT("test_value", 42.0);
    REQUIRE(true);
}

TEST_CASE("TracyIntegration: alloc/free macros compile to no-op", "[tracy]")
{
    int dummy_buffer = 0;
    MARKAMP_TRACY_ALLOC(&dummy_buffer, sizeof(dummy_buffer));
    MARKAMP_TRACY_FREE(&dummy_buffer);
    REQUIRE(true);
}

TEST_CASE("TracyIntegration: message macro compiles to no-op", "[tracy]")
{
    const std::string message = "test message";
    MARKAMP_TRACY_MESSAGE(message.c_str(), message.size());
    REQUIRE(true);
}

TEST_CASE("TracyIntegration: macros can be used in loops without warnings", "[tracy]")
{
    for (int iteration = 0; iteration < 3; ++iteration)
    {
        MARKAMP_TRACY_ZONE("loop_body");
        MARKAMP_TRACY_PLOT("iteration_count", static_cast<double>(iteration));
    }
    REQUIRE(true);
}

TEST_CASE("TracyIntegration: macros can be nested", "[tracy]")
{
    MARKAMP_TRACY_ZONE("outer_zone");
    {
        MARKAMP_TRACY_ZONE("inner_zone");
        REQUIRE(true);
    }
    REQUIRE(true);
}
