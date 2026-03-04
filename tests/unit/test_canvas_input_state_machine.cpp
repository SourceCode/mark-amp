/**
 * @file test_canvas_input_state_machine.cpp
 * @brief Phase 41: Tests for CanvasToolStateMachine and PointerEventRouter.
 */

#include "canvas/CanvasToolStateMachine.h"
#include "canvas/PointerEventRouter.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ═══════════════════════════════════════════════════════
// CanvasToolStateMachine
// ═══════════════════════════════════════════════════════

TEST_CASE("CanvasToolStateMachine - initial state", "[canvas][input]")
{
    CanvasToolStateMachine sm;
    CHECK(sm.current_state() == ToolState::kIdle);
    CHECK(CanvasToolStateMachine::state_name(ToolState::kIdle) == "idle");
}

TEST_CASE("CanvasToolStateMachine - valid transitions", "[canvas][input]")
{
    CanvasToolStateMachine sm;

    CHECK(sm.transition_to(ToolState::kHover));
    CHECK(sm.current_state() == ToolState::kHover);

    CHECK(sm.transition_to(ToolState::kPressed));
    CHECK(sm.transition_to(ToolState::kDrag));
    CHECK(sm.transition_to(ToolState::kCommit));
    CHECK(sm.transition_to(ToolState::kIdle));
    CHECK(sm.transition_count() == 5);
}

TEST_CASE("CanvasToolStateMachine - invalid transitions", "[canvas][input]")
{
    CanvasToolStateMachine sm;

    CHECK_FALSE(sm.transition_to(ToolState::kDrag));   // idle->drag invalid
    CHECK_FALSE(sm.transition_to(ToolState::kCommit)); // idle->commit invalid
    CHECK(sm.current_state() == ToolState::kIdle);
}

TEST_CASE("CanvasToolStateMachine - cancel path", "[canvas][input]")
{
    CanvasToolStateMachine sm;
    sm.transition_to(ToolState::kHover);
    sm.transition_to(ToolState::kPressed);
    sm.transition_to(ToolState::kDrag);

    CHECK(sm.transition_to(ToolState::kCancel));
    CHECK(sm.current_state() == ToolState::kCancel);

    CHECK(sm.transition_to(ToolState::kIdle));
    CHECK(sm.current_state() == ToolState::kIdle);
}

TEST_CASE("CanvasToolStateMachine - modifiers", "[canvas][input]")
{
    CanvasToolStateMachine sm;
    sm.set_modifiers(static_cast<uint8_t>(ModifierKey::kShift) |
                     static_cast<uint8_t>(ModifierKey::kCtrl));

    CHECK(sm.has_modifier(ModifierKey::kShift));
    CHECK(sm.has_modifier(ModifierKey::kCtrl));
    CHECK_FALSE(sm.has_modifier(ModifierKey::kAlt));
    CHECK(sm.modifier_description() == "Shift+Ctrl");
}

TEST_CASE("CanvasToolStateMachine - no modifiers", "[canvas][input]")
{
    CanvasToolStateMachine sm;
    CHECK(sm.modifier_description() == "none");
}

TEST_CASE("CanvasToolStateMachine - safe to switch", "[canvas][input]")
{
    CanvasToolStateMachine sm;
    CHECK(sm.is_safe_to_switch());

    sm.transition_to(ToolState::kHover);
    CHECK(sm.is_safe_to_switch());

    sm.transition_to(ToolState::kPressed);
    CHECK_FALSE(sm.is_safe_to_switch());
}

TEST_CASE("CanvasToolStateMachine - force cancel for switch", "[canvas][input]")
{
    CanvasToolStateMachine sm;
    sm.transition_to(ToolState::kHover);
    sm.transition_to(ToolState::kPressed);
    sm.transition_to(ToolState::kDrag);

    CHECK(sm.force_cancel_for_switch());
    CHECK(sm.current_state() == ToolState::kIdle);
}

TEST_CASE("CanvasToolStateMachine - reset", "[canvas][input]")
{
    CanvasToolStateMachine sm;
    sm.transition_to(ToolState::kHover);
    sm.set_modifiers(static_cast<uint8_t>(ModifierKey::kShift));

    sm.reset();
    CHECK(sm.current_state() == ToolState::kIdle);
    CHECK_FALSE(sm.has_modifier(ModifierKey::kShift));
}

// ═══════════════════════════════════════════════════════
// PointerEvent
// ═══════════════════════════════════════════════════════

TEST_CASE("PointerEvent - type name", "[canvas][pointer]")
{
    PointerEvent event;
    event.type = PointerEventType::kDown;
    CHECK(event.type_name() == "down");

    event.type = PointerEventType::kCancel;
    CHECK(event.type_name() == "cancel");
}

// ═══════════════════════════════════════════════════════
// HitTarget
// ═══════════════════════════════════════════════════════

TEST_CASE("HitTarget - contains", "[canvas][pointer]")
{
    HitTarget target;
    target.x = 10.0;
    target.y = 10.0;
    target.width = 100.0;
    target.height = 50.0;

    CHECK(target.contains(50.0, 30.0));
    CHECK_FALSE(target.contains(5.0, 5.0));
}

// ═══════════════════════════════════════════════════════
// PointerEventRouter
// ═══════════════════════════════════════════════════════

TEST_CASE("PointerEventRouter - register and hit test", "[canvas][pointer]")
{
    PointerEventRouter router;
    router.register_target({"btn1", 1, 0.0, 0.0, 100.0, 50.0});
    router.register_target({"btn2", 2, 50.0, 0.0, 100.0, 50.0});

    CHECK(router.target_count() == 2);

    // At (75,25), btn2 overlaps and has higher z-order
    const auto* hit = router.hit_test(75.0, 25.0);
    REQUIRE(hit != nullptr);
    CHECK(hit->target_id == "btn2");
}

TEST_CASE("PointerEventRouter - dispatch with capture", "[canvas][pointer]")
{
    PointerEventRouter router;
    router.register_target({"panel", 1, 0.0, 0.0, 200.0, 200.0});

    // Down captures
    PointerEvent down;
    down.type = PointerEventType::kDown;
    down.x = 50.0;
    down.y = 50.0;
    auto target = router.dispatch(down);
    CHECK(target == "panel");
    CHECK(router.has_capture());

    // Move routes to captured target
    PointerEvent move;
    move.type = PointerEventType::kMove;
    move.x = 500.0; // Even outside the target
    move.y = 500.0;
    target = router.dispatch(move);
    CHECK(target == "panel");

    // Up releases capture
    PointerEvent up;
    up.type = PointerEventType::kUp;
    target = router.dispatch(up);
    CHECK(target == "panel");
    CHECK_FALSE(router.has_capture());
}

TEST_CASE("PointerEventRouter - cancel releases capture", "[canvas][pointer]")
{
    PointerEventRouter router;
    router.register_target({"panel", 1, 0.0, 0.0, 200.0, 200.0});

    PointerEvent down;
    down.type = PointerEventType::kDown;
    down.x = 50.0;
    down.y = 50.0;
    router.dispatch(down);
    CHECK(router.has_capture());

    PointerEvent cancel;
    cancel.type = PointerEventType::kCancel;
    router.dispatch(cancel);
    CHECK_FALSE(router.has_capture());
}

TEST_CASE("PointerEventRouter - remove target", "[canvas][pointer]")
{
    PointerEventRouter router;
    router.register_target({"a", 1, 0.0, 0.0, 100.0, 100.0});
    router.remove_target("a");
    CHECK(router.target_count() == 0);
}
