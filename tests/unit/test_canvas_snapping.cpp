/**
 * @file test_canvas_snapping.cpp
 * @brief Phase 49: Tests for SnapTargetEngine and GridGuideController.
 */

#include "canvas/GridGuideController.h"
#include "canvas/SnapTargetEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ═══════════════════════════════════════════════════════
// SnapCandidate
// ═══════════════════════════════════════════════════════

TEST_CASE("SnapCandidate - type name", "[canvas][snap]")
{
    SnapCandidate cand;
    cand.type = SnapTarget::kGrid;
    CHECK(cand.type_name() == "grid");

    cand.type = SnapTarget::kObjectCenter;
    CHECK(cand.type_name() == "object_center");
}

// ═══════════════════════════════════════════════════════
// SnapTargetEngine
// ═══════════════════════════════════════════════════════

TEST_CASE("SnapTargetEngine - tolerance", "[canvas][snap]")
{
    SnapTargetEngine engine;
    CHECK(engine.tolerance() == 8.0);
    engine.set_tolerance(12.0);
    CHECK(engine.tolerance() == 12.0);
}

TEST_CASE("SnapTargetEngine - target enable/disable", "[canvas][snap]")
{
    SnapTargetEngine engine;
    CHECK(engine.is_target_enabled(SnapTarget::kGrid));
    engine.set_target_enabled(SnapTarget::kGrid, false);
    CHECK_FALSE(engine.is_target_enabled(SnapTarget::kGrid));
}

TEST_CASE("SnapTargetEngine - resolve best snap", "[canvas][snap]")
{
    SnapTargetEngine engine;
    engine.set_tolerance(10.0);

    SnapCandidate close;
    close.type = SnapTarget::kGrid;
    close.snap_x = 102.0;
    close.snap_y = 100.0;
    engine.add_candidate(close);

    SnapCandidate far;
    far.type = SnapTarget::kObjectEdge;
    far.snap_x = 200.0;
    far.snap_y = 200.0;
    engine.add_candidate(far);

    auto best = engine.resolve(100.0, 100.0);
    CHECK(best.snap_x == 102.0);
    CHECK(engine.has_snap(100.0, 100.0));
    CHECK_FALSE(engine.has_snap(500.0, 500.0));
}

TEST_CASE("SnapTargetEngine - disabled type skipped", "[canvas][snap]")
{
    SnapTargetEngine engine;
    engine.set_tolerance(10.0);

    SnapCandidate grid_cand;
    grid_cand.type = SnapTarget::kGrid;
    grid_cand.snap_x = 100.0;
    grid_cand.snap_y = 100.0;
    engine.add_candidate(grid_cand);

    engine.set_target_enabled(SnapTarget::kGrid, false);
    CHECK_FALSE(engine.has_snap(100.0, 100.0));
}

TEST_CASE("SnapTargetEngine - equal spacing detection", "[canvas][snap]")
{
    std::vector<double> positions = {10.0, 30.0, 50.0, 70.0};
    auto hint = SnapTargetEngine::detect_equal_spacing(positions, 0.5);

    CHECK(hint.is_equal);
    CHECK(hint.spacing == 20.0);
    CHECK(hint.match_count == 3);
}

TEST_CASE("SnapTargetEngine - unequal spacing", "[canvas][snap]")
{
    std::vector<double> positions = {10.0, 30.0, 60.0};
    auto hint = SnapTargetEngine::detect_equal_spacing(positions, 0.5);
    CHECK_FALSE(hint.is_equal);
}

// ═══════════════════════════════════════════════════════
// GridGuideController
// ═══════════════════════════════════════════════════════

TEST_CASE("GridConfig - style name", "[canvas][grid]")
{
    GridConfig config;
    config.style = GridStyle::kLines;
    CHECK(config.style_name() == "lines");
}

TEST_CASE("GridGuideController - snap to grid", "[canvas][grid]")
{
    GridGuideController ctrl;
    GridConfig config;
    config.spacing = 10.0;
    ctrl.set_grid(config);

    CHECK(ctrl.snap_to_grid(13.0) == 10.0);
    CHECK(ctrl.snap_to_grid(17.0) == 20.0);
    CHECK(ctrl.snap_to_grid(15.0) == 20.0); // rounds up at midpoint
}

TEST_CASE("GridGuideController - guide CRUD", "[canvas][grid]")
{
    GridGuideController ctrl;
    ctrl.add_guide({"g1", true, 100.0, false});
    ctrl.add_guide({"g2", false, 200.0, false});
    CHECK(ctrl.guide_count() == 2);

    ctrl.remove_guide("g1");
    CHECK(ctrl.guide_count() == 1);

    ctrl.set_guide_locked("g2", true);
    CHECK(ctrl.guides()[0].locked);

    ctrl.clear_guides();
    CHECK(ctrl.guide_count() == 0);
}

TEST_CASE("GridGuideController - grid visibility", "[canvas][grid]")
{
    GridGuideController ctrl;
    CHECK(ctrl.grid_visible());
    ctrl.set_grid_visible(false);
    CHECK_FALSE(ctrl.grid_visible());
}
