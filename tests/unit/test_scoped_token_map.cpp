#include "core/ScopedTokenMap.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::core;

TEST_CASE("ScopedTokenMap can store and retrieve exact tokens", "[theme][v2]")
{
    ScopedTokenMap map;
    map.set("editor.background", wxColour(30, 30, 30));
    map.set("editor.foreground", wxColour(200, 200, 200));

    auto bg = map.resolve("editor.background");
    REQUIRE(bg.has_value());
    REQUIRE(bg->Red() == 30);
    REQUIRE(bg->Green() == 30);
    REQUIRE(bg->Blue() == 30);

    auto fg = map.resolve("editor.foreground");
    REQUIRE(fg.has_value());
    REQUIRE(fg->Red() == 200);

    auto missing = map.resolve("editor.missing");
    REQUIRE(!missing.has_value());
}

TEST_CASE("TokenInheritanceResolver handles fallbacks", "[theme][v2]")
{
    ScopedTokenMap map;
    map.set("editor.background", wxColour(30, 30, 30));
    map.set("window.background", wxColour(20, 20, 20));

    // Explicit value takes precedence
    map.set("tab.activeBackground", wxColour(40, 40, 40));

    // Add fallback chain
    map.resolver().register_fallback("tab.activeBackground", "editor.background");
    map.resolver().register_fallback("tab.inactiveBackground", "editor.background");
    map.resolver().register_fallback("missing.token", "also.missing");
    map.resolver().register_fallback("also.missing", "window.background");
    map.resolver().register_fallback("cyclic.a", "cyclic.b");
    map.resolver().register_fallback("cyclic.b", "cyclic.a");

    SECTION("Explicit value")
    {
        auto active = map.resolve("tab.activeBackground");
        REQUIRE(active.has_value());
        REQUIRE(active->Red() == 40);
    }

    SECTION("Fallback lookup")
    {
        auto inactive = map.resolve("tab.inactiveBackground");
        REQUIRE(inactive.has_value());
        REQUIRE(inactive->Red() == 30); // falls back to editor.background
    }

    SECTION("Deep fallback")
    {
        // missing.token -> also.missing (not set) -> window.background (set)
        auto missing = map.resolve("missing.token");
        REQUIRE(missing.has_value());
        REQUIRE(missing->Red() == 20);
    }

    SECTION("Missing entirely")
    {
        auto none = map.resolve("totally.unknown");
        REQUIRE(!none.has_value());
    }

    SECTION("Cyclic dependency prevention")
    {
        // Should not hang, should return empty or error
        auto cyclic = map.resolve("cyclic.a");
        REQUIRE(!cyclic.has_value());
    }
}
