/// test_surface_link.cpp — Unit tests
#include "core/SurfaceLink.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("SurfaceKind: enum values", "[surface_link]")
{
    REQUIRE(static_cast<uint8_t>(SurfaceKind::kEditor) != static_cast<uint8_t>(SurfaceKind::kCanvas));
}

TEST_CASE("SurfaceLink: compiles", "[surface_link]")
{
    static_assert(sizeof(SurfaceLink) > 0);
}

TEST_CASE("LinkAnchor: compiles", "[surface_link]")
{
    static_assert(sizeof(LinkAnchor) > 0);
}
