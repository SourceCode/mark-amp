/**
 * @file test_drag_drop_controls.cpp
 * @brief Phase 32: Unit tests for DragAffordanceController and DropTargetHighlighter.
 */

#include "ui/DragAffordanceController.h"
#include "ui/DropTargetHighlighter.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// DragIntent
// ═══════════════════════════════════════════════════════

TEST_CASE("DragIntent - meets threshold", "[drag_drop][intent]")
{
    DragIntent intent;
    intent.distance_px = 10;
    intent.hold_time_ms = 100;

    CHECK(intent.meets_threshold(5, 50));
    CHECK(intent.meets_threshold(10, 100));
    CHECK_FALSE(intent.meets_threshold(15, 50));
    CHECK_FALSE(intent.meets_threshold(5, 200));
}

// ═══════════════════════════════════════════════════════
// DragAffordanceController — lifecycle
// ═══════════════════════════════════════════════════════

TEST_CASE("DragAffordanceController - initial state", "[drag_drop][affordance]")
{
    DragAffordanceController ctrl;
    CHECK(ctrl.state() == DragState::kIdle);
    CHECK_FALSE(ctrl.is_dragging());
    CHECK(ctrl.distance_threshold() == 5);
    CHECK(ctrl.hold_threshold() == 0);
    CHECK_FALSE(ctrl.is_copy_mode());
}

TEST_CASE("DragAffordanceController - begin and update", "[drag_drop][affordance]")
{
    DragAffordanceController ctrl;
    ctrl.begin("tab_1");
    CHECK(ctrl.state() == DragState::kPending);
    CHECK(ctrl.source_id() == "tab_1");

    ctrl.update(3);
    CHECK(ctrl.state() == DragState::kPending); // Below threshold

    ctrl.update(3);
    CHECK(ctrl.state() == DragState::kDragging); // Above threshold
    CHECK(ctrl.is_dragging());
}

TEST_CASE("DragAffordanceController - cancel", "[drag_drop][affordance]")
{
    DragAffordanceController ctrl;
    ctrl.begin("tab_1");
    ctrl.update(10);
    CHECK(ctrl.is_dragging());

    ctrl.cancel();
    CHECK(ctrl.state() == DragState::kCancelled);
    CHECK_FALSE(ctrl.is_dragging());
}

TEST_CASE("DragAffordanceController - complete valid drop", "[drag_drop][affordance]")
{
    DragAffordanceController ctrl;
    ctrl.set_drop_zones({{"zone_a", "Panel A", DropValidity::kValid}});
    ctrl.begin("tab_1");
    ctrl.update(10);

    bool result = ctrl.complete("zone_a");
    CHECK(result);
    CHECK(ctrl.state() == DragState::kCompleted);
}

TEST_CASE("DragAffordanceController - complete invalid drop", "[drag_drop][affordance]")
{
    DragAffordanceController ctrl;
    ctrl.set_drop_zones({{"zone_a", "Panel A", DropValidity::kInvalid}});
    ctrl.begin("tab_1");
    ctrl.update(10);

    bool result = ctrl.complete("zone_a");
    CHECK_FALSE(result);
    CHECK_FALSE(ctrl.last_error().reason.empty());
    CHECK(ctrl.last_error().zone_id == "zone_a");
}

TEST_CASE("DragAffordanceController - reset", "[drag_drop][affordance]")
{
    DragAffordanceController ctrl;
    ctrl.begin("tab_1");
    ctrl.update(10);
    ctrl.set_copy_mode(true);

    ctrl.reset();
    CHECK(ctrl.state() == DragState::kIdle);
    CHECK_FALSE(ctrl.is_copy_mode());
}

// ═══════════════════════════════════════════════════════
// DragAffordanceController — cursor states
// ═══════════════════════════════════════════════════════

TEST_CASE("DragAffordanceController - cursor idle", "[drag_drop][cursor]")
{
    DragAffordanceController ctrl;
    CHECK(ctrl.cursor_state() == DragCursorState::kDefault);
}

TEST_CASE("DragAffordanceController - cursor pending", "[drag_drop][cursor]")
{
    DragAffordanceController ctrl;
    ctrl.begin("tab_1");
    CHECK(ctrl.cursor_state() == DragCursorState::kGrab);
}

TEST_CASE("DragAffordanceController - cursor dragging", "[drag_drop][cursor]")
{
    DragAffordanceController ctrl;
    ctrl.begin("tab_1");
    ctrl.update(10);
    CHECK(ctrl.cursor_state() == DragCursorState::kGrabbing);
}

TEST_CASE("DragAffordanceController - cursor over valid zone", "[drag_drop][cursor]")
{
    DragAffordanceController ctrl;
    ctrl.set_drop_zones({{"zone_a", "Panel A", DropValidity::kValid}});
    ctrl.begin("tab_1");
    ctrl.update(10);

    CHECK(ctrl.cursor_state("zone_a") == DragCursorState::kMove);
}

TEST_CASE("DragAffordanceController - cursor over valid zone with copy", "[drag_drop][cursor]")
{
    DragAffordanceController ctrl;
    ctrl.set_drop_zones({{"zone_a", "Panel A", DropValidity::kValid}});
    ctrl.begin("tab_1");
    ctrl.update(10);
    ctrl.set_copy_mode(true);

    CHECK(ctrl.cursor_state("zone_a") == DragCursorState::kCopy);
}

TEST_CASE("DragAffordanceController - cursor over invalid zone", "[drag_drop][cursor]")
{
    DragAffordanceController ctrl;
    ctrl.set_drop_zones({{"zone_a", "Panel A", DropValidity::kInvalid}});
    ctrl.begin("tab_1");
    ctrl.update(10);

    CHECK(ctrl.cursor_state("zone_a") == DragCursorState::kNoDrop);
}

// ═══════════════════════════════════════════════════════
// DragAffordanceController — intent evaluation
// ═══════════════════════════════════════════════════════

TEST_CASE("DragAffordanceController - evaluate intent", "[drag_drop][intent]")
{
    DragAffordanceController ctrl;
    ctrl.set_distance_threshold(8);
    ctrl.set_hold_threshold(100);

    DragIntent good;
    good.distance_px = 10;
    good.hold_time_ms = 150;
    CHECK(ctrl.evaluate_intent(good));

    DragIntent bad;
    bad.distance_px = 3;
    bad.hold_time_ms = 50;
    CHECK_FALSE(ctrl.evaluate_intent(bad));
}

// ═══════════════════════════════════════════════════════
// DropTargetHighlighter — zone management
// ═══════════════════════════════════════════════════════

TEST_CASE("DropTargetHighlighter - empty state", "[drag_drop][highlight]")
{
    DropTargetHighlighter hl;
    CHECK(hl.zone_count() == 0);
    CHECK_FALSE(hl.has_highlights());
}

TEST_CASE("DropTargetHighlighter - add and remove zones", "[drag_drop][highlight]")
{
    DropTargetHighlighter hl;
    hl.add_zone("panel_left");
    hl.add_zone("panel_bottom");
    CHECK(hl.zone_count() == 2);

    hl.remove_zone("panel_left");
    CHECK(hl.zone_count() == 1);

    hl.clear_zones();
    CHECK(hl.zone_count() == 0);
}

TEST_CASE("DropTargetHighlighter - highlight valid", "[drag_drop][highlight]")
{
    DropTargetHighlighter hl;
    hl.add_zone("panel_left");
    hl.highlight_valid("panel_left", InsertionPosition::kBefore);

    auto state = hl.zone_state("panel_left");
    CHECK(state.style == HighlightStyle::kValid);
    CHECK(state.insertion == InsertionPosition::kBefore);
    CHECK(hl.has_highlights());
}

TEST_CASE("DropTargetHighlighter - highlight invalid", "[drag_drop][highlight]")
{
    DropTargetHighlighter hl;
    hl.add_zone("panel_left");
    hl.highlight_invalid("panel_left");

    auto state = hl.zone_state("panel_left");
    CHECK(state.style == HighlightStyle::kInvalid);
    CHECK(state.insertion == InsertionPosition::kNone);
}

TEST_CASE("DropTargetHighlighter - hover tracking", "[drag_drop][highlight]")
{
    DropTargetHighlighter hl;
    hl.add_zone("zone_a");
    hl.add_zone("zone_b");

    hl.set_hovered("zone_a");
    CHECK(hl.zone_state("zone_a").is_hovered);
    CHECK_FALSE(hl.zone_state("zone_b").is_hovered);

    // Hovering zone_b clears zone_a
    hl.set_hovered("zone_b");
    CHECK_FALSE(hl.zone_state("zone_a").is_hovered);
    CHECK(hl.zone_state("zone_b").is_hovered);
}

TEST_CASE("DropTargetHighlighter - reset all", "[drag_drop][highlight]")
{
    DropTargetHighlighter hl;
    hl.add_zone("zone_a");
    hl.highlight_valid("zone_a");
    hl.set_hovered("zone_a");

    hl.reset_all();
    auto state = hl.zone_state("zone_a");
    CHECK(state.style == HighlightStyle::kNone);
    CHECK_FALSE(state.is_hovered);
    CHECK(hl.zone_count() == 1); // Zones still registered
}

TEST_CASE("DropTargetHighlighter - highlighted zones filter", "[drag_drop][highlight]")
{
    DropTargetHighlighter hl;
    hl.add_zone("zone_a");
    hl.add_zone("zone_b");
    hl.add_zone("zone_c");
    hl.highlight_valid("zone_a");
    hl.highlight_invalid("zone_c");

    auto highlighted = hl.highlighted_zones();
    CHECK(highlighted.size() == 2);
}

TEST_CASE("DropTargetHighlighter - style and insertion names", "[drag_drop][highlight]")
{
    DropTargetState state;
    state.style = HighlightStyle::kValid;
    state.insertion = InsertionPosition::kAfter;
    CHECK(state.style_name() == "valid");
    CHECK(state.insertion_name() == "after");

    state.style = HighlightStyle::kNeutral;
    state.insertion = InsertionPosition::kInside;
    CHECK(state.style_name() == "neutral");
    CHECK(state.insertion_name() == "inside");
}

TEST_CASE("DropError - make error", "[drag_drop][error]")
{
    auto error = DragAffordanceController::make_error("zone_x", "Incompatible type");
    CHECK(error.zone_id == "zone_x");
    CHECK(error.reason == "Incompatible type");
    CHECK_FALSE(error.suggestion.empty());
}
