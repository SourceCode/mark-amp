#include "canvas/MinimapPanel.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::canvas;

static constexpr double kEpsilon = 0.01;

TEST_CASE("MinimapPanel: world_to_minimap", "[minimap]")
{
    MinimapPanel panel;
    panel.set_panel_size(200.0, 100.0);
    panel.set_content_bounds({0.0, 0.0, 1000.0, 500.0});

    // Scale = min(200*0.9 / 1000, 100*0.9 / 500) = min(0.18, 0.18) = 0.18
    auto point = panel.world_to_minimap({500.0, 250.0});
    // Expected: margin_x=10, margin_y=5, x= 10 + 500*0.18 = 100, y= 5 + 250*0.18 = 50
    REQUIRE_THAT(point.x, Catch::Matchers::WithinAbs(100.0, kEpsilon));
    REQUIRE_THAT(point.y, Catch::Matchers::WithinAbs(50.0, kEpsilon));
}

TEST_CASE("MinimapPanel: minimap_to_world roundtrip", "[minimap]")
{
    MinimapPanel panel;
    panel.set_panel_size(200.0, 100.0);
    panel.set_content_bounds({0.0, 0.0, 1000.0, 500.0});

    Point2D original{300.0, 200.0};
    auto minimap = panel.world_to_minimap(original);
    auto roundtrip = panel.minimap_to_world(minimap);

    REQUIRE_THAT(roundtrip.x, Catch::Matchers::WithinAbs(original.x, kEpsilon));
    REQUIRE_THAT(roundtrip.y, Catch::Matchers::WithinAbs(original.y, kEpsilon));
}

TEST_CASE("MinimapPanel: click navigation fires callback", "[minimap]")
{
    MinimapPanel panel;
    panel.set_panel_size(200.0, 100.0);
    panel.set_content_bounds({0.0, 0.0, 1000.0, 500.0});

    Point2D navigated{0.0, 0.0};
    panel.set_on_navigate([&](const Point2D& world) { navigated = world; });

    REQUIRE(panel.handle_click(100.0, 50.0));
    REQUIRE_THAT(navigated.x, Catch::Matchers::WithinAbs(500.0, kEpsilon));
    REQUIRE_THAT(navigated.y, Catch::Matchers::WithinAbs(250.0, kEpsilon));
}

TEST_CASE("MinimapPanel: viewport indicator position", "[minimap]")
{
    MinimapPanel panel;
    panel.set_panel_size(200.0, 100.0);
    panel.set_content_bounds({0.0, 0.0, 1000.0, 500.0});
    panel.set_viewport({100.0, 50.0, 600.0, 300.0});

    auto vp_min = panel.world_to_minimap({100.0, 50.0});
    auto vp_max = panel.world_to_minimap({600.0, 300.0});

    // Both corners should be within panel bounds.
    REQUIRE(vp_min.x >= 0.0);
    REQUIRE(vp_min.y >= 0.0);
    REQUIRE(vp_max.x <= 200.0);
    REQUIRE(vp_max.y <= 100.0);
    REQUIRE(vp_max.x > vp_min.x);
    REQUIRE(vp_max.y > vp_min.y);
}

TEST_CASE("MinimapPanel: scale computation for wide content", "[minimap]")
{
    MinimapPanel panel;
    panel.set_panel_size(100.0, 200.0);                  // tall panel
    panel.set_content_bounds({0.0, 0.0, 2000.0, 500.0}); // wide content

    double scale = panel.compute_scale();
    double usable_w = 100.0 * 0.9;
    double expected = usable_w / 2000.0; // width-limited
    REQUIRE_THAT(scale, Catch::Matchers::WithinAbs(expected, 0.0001));
}

TEST_CASE("MinimapPanel: entries within panel bounds", "[minimap]")
{
    MinimapPanel panel;
    panel.set_panel_size(200.0, 150.0);
    panel.set_content_bounds({0.0, 0.0, 1000.0, 1000.0});

    std::vector<MinimapEntry> entries = {
        {{100.0, 100.0, 200.0, 200.0}, {255, 0, 0, 255}, CanvasObjectType::StickyNote},
        {{500.0, 500.0, 600.0, 600.0}, {0, 255, 0, 255}, CanvasObjectType::TextBox},
        {{800.0, 800.0, 900.0, 900.0}, {0, 0, 255, 255}, CanvasObjectType::Shape},
    };
    panel.set_entries(entries);

    for (const auto& entry : panel.entries())
    {
        auto min_pt = panel.world_to_minimap({entry.bounds.min_x, entry.bounds.min_y});
        auto max_pt = panel.world_to_minimap({entry.bounds.max_x, entry.bounds.max_y});
        REQUIRE(min_pt.x >= 0.0);
        REQUIRE(min_pt.y >= 0.0);
        REQUIRE(max_pt.x <= panel.panel_width());
        REQUIRE(max_pt.y <= panel.panel_height());
    }
}
