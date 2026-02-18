/// test_dirty_region.cpp — Unit tests
#include "rendering/DirtyRegion.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::rendering;

TEST_CASE("Rect: default is empty", "[dirty_region]")
{
    Rect r;
    REQUIRE(r.empty());
}

TEST_CASE("Rect: non-empty when has extent", "[dirty_region]")
{
    Rect r{0, 0, 100, 50};
    REQUIRE_FALSE(r.empty());
    REQUIRE(r.width() == 100);
    REQUIRE(r.height() == 50);
}

TEST_CASE("Rect: intersects", "[dirty_region]")
{
    Rect a{0, 0, 50, 50};
    Rect b{25, 25, 75, 75};
    REQUIRE(a.intersects(b));
}

TEST_CASE("Rect: does not intersect", "[dirty_region]")
{
    Rect a{0, 0, 10, 10};
    Rect b{20, 20, 30, 30};
    REQUIRE_FALSE(a.intersects(b));
}

TEST_CASE("Rect: united", "[dirty_region]")
{
    Rect a{0, 0, 10, 10};
    Rect b{5, 5, 15, 15};
    auto u = a.united(b);
    REQUIRE(u.left == 0);
    REQUIRE(u.top == 0);
    REQUIRE(u.right == 15);
    REQUIRE(u.bottom == 15);
}

TEST_CASE("DirtyRegionAccumulator: starts clean", "[dirty_region]")
{
    DirtyRegionAccumulator acc;
    REQUIRE_FALSE(acc.has_dirty());
}

TEST_CASE("DirtyRegionAccumulator: invalidate marks dirty", "[dirty_region]")
{
    DirtyRegionAccumulator acc;
    acc.invalidate({10, 10, 100, 50});
    REQUIRE(acc.has_dirty());
    REQUIRE(acc.count() >= 1);
}

TEST_CASE("DirtyRegionAccumulator: clear resets", "[dirty_region]")
{
    DirtyRegionAccumulator acc;
    acc.invalidate({0, 0, 100, 100});
    acc.clear();
    REQUIRE_FALSE(acc.has_dirty());
}

TEST_CASE("DirtyRegionAccumulator: consume returns rects and clears", "[dirty_region]")
{
    DirtyRegionAccumulator acc;
    acc.invalidate({0, 0, 50, 50});
    auto rects = acc.consume();
    REQUIRE_FALSE(rects.empty());
    REQUIRE_FALSE(acc.has_dirty());
}
