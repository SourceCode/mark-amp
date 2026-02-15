#include "canvas/ViewportTransform.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::canvas;

TEST_CASE("ViewportTransform default state", "[canvas][viewport]")
{
    ViewportTransform vt;

    SECTION("default zoom is 100%")
    {
        REQUIRE(vt.zoom() == 1.0);
    }

    SECTION("default pan is origin")
    {
        REQUIRE(vt.pan().x == 0.0);
        REQUIRE(vt.pan().y == 0.0);
    }

    SECTION("identity mapping at default state")
    {
        const Point2D world{42.0, 17.0};
        const auto screen = vt.world_to_screen(world);
        REQUIRE_THAT(screen.x, Catch::Matchers::WithinAbs(42.0, 1e-9));
        REQUIRE_THAT(screen.y, Catch::Matchers::WithinAbs(17.0, 1e-9));
    }
}

TEST_CASE("ViewportTransform coordinate conversion", "[canvas][viewport]")
{
    ViewportTransform vt;
    vt.set_screen_size(800.0, 600.0);

    SECTION("pan offsets world coordinates")
    {
        vt.set_pan({100.0, 50.0});
        const auto screen = vt.world_to_screen(Point2D{200.0, 100.0});
        REQUIRE_THAT(screen.x, Catch::Matchers::WithinAbs(100.0, 1e-9));
        REQUIRE_THAT(screen.y, Catch::Matchers::WithinAbs(50.0, 1e-9));
    }

    SECTION("zoom scales coordinates")
    {
        vt.set_zoom(2.0);
        const auto screen = vt.world_to_screen(Point2D{50.0, 30.0});
        REQUIRE_THAT(screen.x, Catch::Matchers::WithinAbs(100.0, 1e-9));
        REQUIRE_THAT(screen.y, Catch::Matchers::WithinAbs(60.0, 1e-9));
    }

    SECTION("roundtrip world -> screen -> world")
    {
        vt.set_zoom(1.5);
        vt.set_pan({200.0, -100.0});

        const Point2D original{42.0, 17.0};
        const auto screen = vt.world_to_screen(Point2D{42.0, 17.0});
        const auto recovered = vt.screen_to_world(screen);

        REQUIRE_THAT(recovered.x, Catch::Matchers::WithinAbs(original.x, 1e-9));
        REQUIRE_THAT(recovered.y, Catch::Matchers::WithinAbs(original.y, 1e-9));
    }

    SECTION("AABB conversion")
    {
        vt.set_zoom(2.0);
        const AABB world{10.0, 10.0, 20.0, 20.0};
        const auto screen = vt.world_to_screen(world);

        REQUIRE_THAT(screen.min_x, Catch::Matchers::WithinAbs(20.0, 1e-9));
        REQUIRE_THAT(screen.min_y, Catch::Matchers::WithinAbs(20.0, 1e-9));
        REQUIRE_THAT(screen.max_x, Catch::Matchers::WithinAbs(40.0, 1e-9));
        REQUIRE_THAT(screen.max_y, Catch::Matchers::WithinAbs(40.0, 1e-9));
    }
}

TEST_CASE("ViewportTransform zoom constraints", "[canvas][viewport]")
{
    ViewportTransform vt;

    SECTION("zoom clamps to minimum")
    {
        vt.set_zoom(0.001);
        REQUIRE_THAT(vt.zoom(), Catch::Matchers::WithinAbs(ViewportTransform::kMinZoom, 1e-9));
    }

    SECTION("zoom clamps to maximum")
    {
        vt.set_zoom(100.0);
        REQUIRE_THAT(vt.zoom(), Catch::Matchers::WithinAbs(ViewportTransform::kMaxZoom, 1e-9));
    }
}

TEST_CASE("ViewportTransform zoom_at preserves cursor world point", "[canvas][viewport]")
{
    ViewportTransform vt;
    vt.set_screen_size(800.0, 600.0);

    // World point under cursor (400, 300) at zoom 1.0.
    const Point2D cursor_screen{400.0, 300.0};
    const auto world_before = vt.screen_to_world(cursor_screen);

    vt.zoom_at(400.0, 300.0, 2.0);

    const auto world_after = vt.screen_to_world(cursor_screen);

    REQUIRE_THAT(world_after.x, Catch::Matchers::WithinAbs(world_before.x, 1e-6));
    REQUIRE_THAT(world_after.y, Catch::Matchers::WithinAbs(world_before.y, 1e-6));
}

TEST_CASE("ViewportTransform visible_region", "[canvas][viewport]")
{
    ViewportTransform vt;
    vt.set_screen_size(800.0, 600.0);
    vt.set_zoom(1.0);
    vt.set_pan({0.0, 0.0});

    const auto region = vt.visible_region();
    REQUIRE_THAT(region.min_x, Catch::Matchers::WithinAbs(0.0, 1e-9));
    REQUIRE_THAT(region.min_y, Catch::Matchers::WithinAbs(0.0, 1e-9));
    REQUIRE_THAT(region.max_x, Catch::Matchers::WithinAbs(800.0, 1e-9));
    REQUIRE_THAT(region.max_y, Catch::Matchers::WithinAbs(600.0, 1e-9));
}

TEST_CASE("ViewportTransform pan_by", "[canvas][viewport]")
{
    ViewportTransform vt;
    vt.set_pan({10.0, 20.0});
    vt.pan_by(5.0, -3.0);
    REQUIRE(vt.pan().x == 15.0);
    REQUIRE(vt.pan().y == 17.0);
}

TEST_CASE("ViewportTransform fit_to_bounds", "[canvas][viewport]")
{
    ViewportTransform vt;
    vt.set_screen_size(800.0, 600.0);

    const AABB content{100.0, 100.0, 500.0, 300.0};
    vt.fit_to_bounds(content, 50.0);

    // After fit, the content should be visible.
    const auto visible = vt.visible_region();
    REQUIRE(visible.contains(content.center()));

    // Zoom should be reasonable (content is 400x200, screen is 700x500 after padding).
    REQUIRE(vt.zoom() > ViewportTransform::kMinZoom);
    REQUIRE(vt.zoom() <= ViewportTransform::kMaxZoom);
}
