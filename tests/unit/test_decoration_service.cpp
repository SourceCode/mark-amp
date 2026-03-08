/// test_decoration_service.cpp — Comprehensive tests for DecorationService
#include "core/DecorationService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("DecorationService: create decoration type", "[decoration_service][positive]")
{
    DecorationService svc;
    DecorationOptions opts;
    opts.background_color = "#ff0000";
    opts.is_whole_line = true;
    auto handle = svc.create_decoration_type(opts);
    REQUIRE(handle != DecorationTypeHandle{});
}

TEST_CASE("DecorationService: set and get decorations", "[decoration_service][positive]")
{
    DecorationService svc;
    DecorationOptions opts;
    opts.background_color = "#00ff00";
    auto handle = svc.create_decoration_type(opts);
    DecorationRange range;
    range.start_line = 5;
    range.end_line = 10;
    svc.set_decorations("file:///main.cpp", handle, {range});
    auto& result = svc.get_decorations("file:///main.cpp", handle);
    REQUIRE(result.size() == 1);
    REQUIRE(result[0].start_line == 5);
}

TEST_CASE("DecorationService: get_decorations for unknown returns empty",
          "[decoration_service][negative]")
{
    DecorationService svc;
    DecorationOptions opts;
    auto handle = svc.create_decoration_type(opts);
    auto& result = svc.get_decorations("file:///unknown.cpp", handle);
    REQUIRE(result.empty());
}

// ── Edge Cases ──

TEST_CASE("DecorationOptions: default values", "[decoration_service][edge]")
{
    DecorationOptions opts;
    REQUIRE(opts.background_color.empty());
    REQUIRE(opts.border_color.empty());
    REQUIRE_FALSE(opts.is_whole_line);
}

TEST_CASE("DecorationRange: default values", "[decoration_service][edge]")
{
    DecorationRange range;
    REQUIRE(range.start_line == 0);
    REQUIRE(range.start_character == 0);
    REQUIRE(range.end_line == 0);
    REQUIRE(range.end_character == 0);
}

TEST_CASE("DecorationService: multiple types coexist", "[decoration_service][edge]")
{
    DecorationService svc;
    DecorationOptions opts1;
    opts1.background_color = "#ff0000";
    DecorationOptions opts2;
    opts2.background_color = "#00ff00";
    auto h1 = svc.create_decoration_type(opts1);
    auto h2 = svc.create_decoration_type(opts2);
    REQUIRE(h1 != h2);
}
