#include "canvas/CanvasTypes.h"
#include "canvas/SnapEngine.h"
#include "canvas/ViewportTransform.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// Helper: create AABB from (min_x, min_y, max_x, max_y).
static auto make_aabb(double minx, double miny, double maxx, double maxy) -> AABB
{
    return AABB{minx, miny, maxx, maxy};
}

// ============================================================================
// Grid Snap tests
// ============================================================================

TEST_CASE("SnapEngine grid snap rounds to nearest grid point", "[canvas][snap]")
{
    SnapEngine engine;
    SnapConfig cfg;
    cfg.grid_enabled = true;
    cfg.grid_spacing = 20.0;
    engine.set_config(cfg);

    const auto snapped = engine.snap_to_grid(Point2D{23.0, 47.0});
    REQUIRE(snapped.x == Catch::Approx(20.0));
    REQUIRE(snapped.y == Catch::Approx(40.0));
}

TEST_CASE("SnapEngine grid snap exact point stays", "[canvas][snap]")
{
    SnapEngine engine;
    SnapConfig cfg;
    cfg.grid_enabled = true;
    cfg.grid_spacing = 10.0;
    engine.set_config(cfg);

    const auto snapped = engine.snap_to_grid(Point2D{30.0, 50.0});
    REQUIRE(snapped.x == Catch::Approx(30.0));
    REQUIRE(snapped.y == Catch::Approx(50.0));
}

TEST_CASE("SnapEngine grid snap disabled returns original", "[canvas][snap]")
{
    SnapEngine engine;
    SnapConfig cfg;
    cfg.grid_enabled = false;
    engine.set_config(cfg);

    const auto snapped = engine.snap_to_grid(Point2D{23.0, 47.0});
    REQUIRE(snapped.x == Catch::Approx(23.0));
    REQUIRE(snapped.y == Catch::Approx(47.0));
}

// ============================================================================
// Object Snap tests
// ============================================================================

TEST_CASE("SnapEngine object snap center alignment", "[canvas][snap]")
{
    SnapEngine engine;
    SnapConfig cfg;
    cfg.object_snap_enabled = true;
    cfg.snap_threshold = 10.0;
    engine.set_config(cfg);

    // Moving object: min(97, 200), max(197, 250) → center(147, 225)
    auto moving = make_aabb(97.0, 200.0, 197.0, 250.0);

    // Other object: min(100, 50), max(200, 100) → center(150, 75)
    // Moving center X=147 is within 10px of 150.
    std::vector<AABB> others = {make_aabb(100.0, 50.0, 200.0, 100.0)};

    const auto result = engine.snap_to_objects(moving, others);
    REQUIRE(result.snapped);
    // Snapped X should center the moving object on the other's center: 150 - 50 = 100
    REQUIRE(result.snapped_x == Catch::Approx(100.0));
    REQUIRE_FALSE(result.guide_lines.empty());
}

TEST_CASE("SnapEngine object snap edge alignment", "[canvas][snap]")
{
    SnapEngine engine;
    SnapConfig cfg;
    cfg.object_snap_enabled = true;
    cfg.snap_threshold = 10.0;
    engine.set_config(cfg);

    // Moving: min(102, 200), max(182, 240). Left edge=102.
    auto moving = make_aabb(102.0, 200.0, 182.0, 240.0);

    // Other: min(100, 50), max(180, 90). Left edge=100.
    // Difference = 2px, within threshold.
    std::vector<AABB> others = {make_aabb(100.0, 50.0, 180.0, 90.0)};

    const auto result = engine.snap_to_objects(moving, others);
    REQUIRE(result.snapped);
    REQUIRE(result.snapped_x == Catch::Approx(100.0));
}

TEST_CASE("SnapEngine object snap no match beyond threshold", "[canvas][snap]")
{
    SnapEngine engine;
    SnapConfig cfg;
    cfg.object_snap_enabled = true;
    cfg.snap_threshold = 5.0;
    engine.set_config(cfg);

    auto moving = make_aabb(50.0, 50.0, 150.0, 100.0);
    std::vector<AABB> others = {make_aabb(300.0, 300.0, 400.0, 350.0)};

    const auto result = engine.snap_to_objects(moving, others);
    REQUIRE_FALSE(result.snapped);
}

TEST_CASE("SnapEngine object snap disabled returns unsnapped", "[canvas][snap]")
{
    SnapEngine engine;
    SnapConfig cfg;
    cfg.object_snap_enabled = false;
    engine.set_config(cfg);

    auto moving = make_aabb(100.0, 100.0, 150.0, 150.0);
    std::vector<AABB> others = {make_aabb(100.0, 50.0, 150.0, 100.0)};

    const auto result = engine.snap_to_objects(moving, others);
    REQUIRE_FALSE(result.snapped);
}

// ============================================================================
// Combined snap_point tests
// ============================================================================

TEST_CASE("SnapEngine snap_point prefers object snap over grid", "[canvas][snap]")
{
    SnapEngine engine;
    SnapConfig cfg;
    cfg.grid_enabled = true;
    cfg.grid_spacing = 20.0;
    cfg.object_snap_enabled = true;
    cfg.snap_threshold = 10.0;
    engine.set_config(cfg);

    ViewportTransform vp;
    Point2D proposed{97.0, 200.0};
    auto moving = make_aabb(97.0, 200.0, 197.0, 250.0);
    std::vector<AABB> others = {make_aabb(100.0, 50.0, 200.0, 100.0)};

    const auto result = engine.snap_point(proposed, moving, others, vp);
    REQUIRE(result.snapped);
    // Object snap should have snapped X to 100 (center align).
    REQUIRE(result.snapped_x == Catch::Approx(100.0));
}

// ============================================================================
// Config tests
// ============================================================================

TEST_CASE("SnapEngine config getters and setters", "[canvas][snap]")
{
    SnapEngine engine;

    REQUIRE(engine.config().grid_enabled);
    REQUIRE(engine.config().grid_spacing == Catch::Approx(20.0));

    SnapConfig cfg;
    cfg.grid_enabled = false;
    cfg.grid_spacing = 50.0;
    cfg.snap_threshold = 12.0;
    engine.set_config(cfg);

    REQUIRE_FALSE(engine.config().grid_enabled);
    REQUIRE(engine.config().grid_spacing == Catch::Approx(50.0));
    REQUIRE(engine.config().snap_threshold == Catch::Approx(12.0));
}
