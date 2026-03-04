/**
 * @file test_canvas_selection_transform.cpp
 * @brief Phase 48: Tests for SelectionSemantics and TransformAlignController.
 */

#include "canvas/SelectionSemantics.h"
#include "canvas/TransformAlignController.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ═══════════════════════════════════════════════════════
// SelectionSemantics
// ═══════════════════════════════════════════════════════

TEST_CASE("SelectionSemantics - mode names", "[canvas][selection]")
{
    CHECK(SelectionSemantics::mode_name(SelectionMode::kClick) == "click");
    CHECK(SelectionSemantics::mode_name(SelectionMode::kMarquee) == "marquee");
}

TEST_CASE("SelectionSemantics - select clears", "[canvas][selection]")
{
    SelectionSemantics sel;
    sel.select({"a", 0, 0, 10, 10});
    sel.select({"b", 20, 0, 10, 10});
    CHECK(sel.count() == 1);
    CHECK(sel.is_selected("b"));
    CHECK_FALSE(sel.is_selected("a"));
}

TEST_CASE("SelectionSemantics - add to selection", "[canvas][selection]")
{
    SelectionSemantics sel;
    sel.select({"a", 0, 0, 10, 10});
    sel.add_to_selection({"b", 20, 0, 10, 10});
    CHECK(sel.count() == 2);
}

TEST_CASE("SelectionSemantics - toggle and deselect", "[canvas][selection]")
{
    SelectionSemantics sel;
    sel.select({"a", 0, 0, 10, 10});
    sel.add_to_selection({"b", 20, 0, 10, 10});
    sel.toggle_selection("a");
    CHECK(sel.count() == 1);
    CHECK_FALSE(sel.is_selected("a"));

    sel.deselect("b");
    CHECK(sel.count() == 0);
}

TEST_CASE("SelectionSemantics - bounds", "[canvas][selection]")
{
    SelectionSemantics sel;
    sel.select({"a", 10.0, 20.0, 30.0, 40.0});
    sel.add_to_selection({"b", 50.0, 10.0, 20.0, 60.0});

    auto bnd = sel.bounds();
    CHECK(bnd.min_x == 10.0);
    CHECK(bnd.min_y == 10.0);
    CHECK(bnd.max_x == 70.0); // 50+20
    CHECK(bnd.max_y == 70.0); // 10+60
    CHECK(bnd.width() == 60.0);
}

// ═══════════════════════════════════════════════════════
// TransformAlignController
// ═══════════════════════════════════════════════════════

TEST_CASE("TransformAlignController - command names", "[canvas][transform]")
{
    CHECK(TransformAlignController::command_name(AlignCommand::kLeft) == "left");
    CHECK(TransformAlignController::command_name(AlignCommand::kCenterH) == "center_h");
}

TEST_CASE("TransformAlignController - align left", "[canvas][transform]")
{
    std::vector<AlignItem> items = {
        {"a", 10, 0, 20, 20},
        {"b", 30, 0, 20, 20},
        {"c", 50, 0, 20, 20},
    };

    auto aligned = TransformAlignController::align(items, AlignCommand::kLeft);
    CHECK(aligned[0].pos_x == 10.0);
    CHECK(aligned[1].pos_x == 10.0);
    CHECK(aligned[2].pos_x == 10.0);
}

TEST_CASE("TransformAlignController - align center H", "[canvas][transform]")
{
    std::vector<AlignItem> items = {
        {"a", 0, 0, 20, 20},
        {"b", 40, 0, 40, 20},
    };

    auto aligned = TransformAlignController::align(items, AlignCommand::kCenterH);
    // Bounds: min=0, max=80, center=40
    CHECK(aligned[0].pos_x == 30.0); // 40 - 20/2
    CHECK(aligned[1].pos_x == 20.0); // 40 - 40/2
}

TEST_CASE("TransformAlignController - distribute horizontal", "[canvas][transform]")
{
    std::vector<AlignItem> items = {
        {"a", 0, 0, 10, 10},
        {"b", 20, 0, 10, 10},
        {"c", 90, 0, 10, 10},
    };

    auto dist = TransformAlignController::distribute(items, DistributeCommand::kHorizontal);
    CHECK(dist.size() == 3);
    // First stays at 0, last ends at 100
    CHECK(dist[0].pos_x == 0.0);
}

TEST_CASE("TransformAlignController - pivot", "[canvas][transform]")
{
    TransformAlignController ctrl;
    ctrl.set_pivot(50.0, 50.0);
    CHECK(ctrl.pivot_x() == 50.0);
    CHECK(ctrl.pivot_y() == 50.0);

    ctrl.reset_pivot();
    CHECK(ctrl.pivot_x() == 0.0);
}
