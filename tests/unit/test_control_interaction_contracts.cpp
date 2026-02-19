// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/ControlState.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ── ControlStateTracker ─────────────────────────────────────────────

TEST_CASE("ControlStateTracker starts in Normal state", "[control][state]")
{
    const ControlStateTracker tracker;
    REQUIRE(tracker.is_normal());
    REQUIRE_FALSE(tracker.is_hover());
    REQUIRE_FALSE(tracker.is_pressed());
    REQUIRE_FALSE(tracker.is_focused());
    REQUIRE_FALSE(tracker.is_disabled());
    REQUIRE_FALSE(tracker.is_selected());
    REQUIRE(tracker.flags() == 0);
}

TEST_CASE("ControlStateTracker pointer transitions: Normal->Hover->Pressed->Hover",
          "[control][state]")
{
    ControlStateTracker tracker;

    tracker.on_mouse_enter();
    REQUIRE(tracker.is_hover());
    REQUIRE_FALSE(tracker.is_pressed());
    REQUIRE(tracker.changed());

    tracker.acknowledge_change();
    tracker.on_mouse_down();
    REQUIRE(tracker.is_pressed());
    REQUIRE_FALSE(tracker.is_hover());
    REQUIRE(tracker.changed());

    tracker.acknowledge_change();
    tracker.on_mouse_up(true); // released inside
    REQUIRE(tracker.is_hover());
    REQUIRE_FALSE(tracker.is_pressed());
    REQUIRE(tracker.changed());
}

TEST_CASE("ControlStateTracker pointer release outside clears hover", "[control][state]")
{
    ControlStateTracker tracker;
    tracker.on_mouse_enter();
    tracker.on_mouse_down();
    tracker.on_mouse_up(false); // released outside
    REQUIRE_FALSE(tracker.is_hover());
    REQUIRE_FALSE(tracker.is_pressed());
}

TEST_CASE("ControlStateTracker mouse leave clears hover and pressed", "[control][state]")
{
    ControlStateTracker tracker;
    tracker.on_mouse_enter();
    tracker.on_mouse_down();

    tracker.on_mouse_leave();
    REQUIRE_FALSE(tracker.is_hover());
    REQUIRE_FALSE(tracker.is_pressed());
}

TEST_CASE("ControlStateTracker focus transitions", "[control][state]")
{
    ControlStateTracker tracker;

    tracker.on_focus();
    REQUIRE(tracker.is_focused());
    REQUIRE(tracker.changed());

    tracker.acknowledge_change();
    tracker.on_blur();
    REQUIRE_FALSE(tracker.is_focused());
    REQUIRE(tracker.changed());
}

TEST_CASE("ControlStateTracker disabled blocks hover and press", "[control][state]")
{
    ControlStateTracker tracker;
    tracker.set_disabled(true);
    REQUIRE(tracker.is_disabled());

    // Hover and press should be blocked
    tracker.on_mouse_enter();
    REQUIRE_FALSE(tracker.is_hover());

    tracker.on_mouse_down();
    REQUIRE_FALSE(tracker.is_pressed());
}

TEST_CASE("ControlStateTracker setting disabled clears interactive flags", "[control][state]")
{
    ControlStateTracker tracker;
    tracker.on_mouse_enter();
    tracker.on_mouse_down();
    REQUIRE(tracker.is_pressed());

    tracker.set_disabled(true);
    REQUIRE_FALSE(tracker.is_hover());
    REQUIRE_FALSE(tracker.is_pressed());
    REQUIRE(tracker.is_disabled());
}

TEST_CASE("ControlStateTracker re-enabling allows interaction", "[control][state]")
{
    ControlStateTracker tracker;
    tracker.set_disabled(true);
    tracker.set_disabled(false);
    REQUIRE_FALSE(tracker.is_disabled());

    tracker.on_mouse_enter();
    REQUIRE(tracker.is_hover());
}

TEST_CASE("ControlStateTracker selected state is persistent", "[control][state]")
{
    ControlStateTracker tracker;
    tracker.set_selected(true);
    REQUIRE(tracker.is_selected());

    // Hover on top of selected
    tracker.on_mouse_enter();
    REQUIRE(tracker.is_selected());
    REQUIRE(tracker.is_hover());

    tracker.on_mouse_leave();
    REQUIRE(tracker.is_selected());
    REQUIRE_FALSE(tracker.is_hover());
}

TEST_CASE("ControlStateTracker reset clears all flags", "[control][state]")
{
    ControlStateTracker tracker;
    tracker.on_mouse_enter();
    tracker.on_focus();
    tracker.set_selected(true);
    tracker.reset();

    REQUIRE(tracker.is_normal());
    REQUIRE(tracker.flags() == 0);
}

TEST_CASE("ControlStateTracker composable states: Focused + Selected + Hover", "[control][state]")
{
    ControlStateTracker tracker;
    tracker.on_focus();
    tracker.set_selected(true);
    tracker.on_mouse_enter();

    REQUIRE(tracker.is_focused());
    REQUIRE(tracker.is_selected());
    REQUIRE(tracker.is_hover());
    REQUIRE(has_flag(tracker.flags(), ControlStateFlag::kFocused));
    REQUIRE(has_flag(tracker.flags(), ControlStateFlag::kSelected));
    REQUIRE(has_flag(tracker.flags(), ControlStateFlag::kHover));
}

// ── IndexedControlState ─────────────────────────────────────────────

TEST_CASE("IndexedControlState starts with no indices", "[control][indexed]")
{
    const IndexedControlState indexed;
    REQUIRE(indexed.hover_index() == -1);
    REQUIRE(indexed.pressed_index() == -1);
    REQUIRE(indexed.focus_index() == -1);
}

TEST_CASE("IndexedControlState set_hover and flags_for", "[control][indexed]")
{
    IndexedControlState indexed;
    indexed.set_hover(2);
    REQUIRE(indexed.hover_index() == 2);

    auto flags = indexed.flags_for(2);
    REQUIRE(has_flag(flags, ControlStateFlag::kHover));
    REQUIRE_FALSE(has_flag(flags, ControlStateFlag::kPressed));

    // Non-hovered item has no flags
    auto other = indexed.flags_for(0);
    REQUIRE(other == 0);
}

TEST_CASE("IndexedControlState pressed and focus", "[control][indexed]")
{
    IndexedControlState indexed;
    indexed.set_pressed(1);
    indexed.set_focus(1);

    auto flags = indexed.flags_for(1);
    REQUIRE(has_flag(flags, ControlStateFlag::kPressed));
    REQUIRE(has_flag(flags, ControlStateFlag::kFocused));
}

TEST_CASE("IndexedControlState clear_transient keeps focus", "[control][indexed]")
{
    IndexedControlState indexed;
    indexed.set_hover(3);
    indexed.set_pressed(3);
    indexed.set_focus(3);

    indexed.clear_transient();
    REQUIRE(indexed.hover_index() == -1);
    REQUIRE(indexed.pressed_index() == -1);
    REQUIRE(indexed.focus_index() == 3); // focus is persistent
}

TEST_CASE("IndexedControlState reset clears everything", "[control][indexed]")
{
    IndexedControlState indexed;
    indexed.set_hover(1);
    indexed.set_pressed(2);
    indexed.set_focus(3);

    indexed.reset();
    REQUIRE(indexed.hover_index() == -1);
    REQUIRE(indexed.pressed_index() == -1);
    REQUIRE(indexed.focus_index() == -1);
}

TEST_CASE("IndexedControlState no-change avoids dirty flag", "[control][indexed]")
{
    IndexedControlState indexed;
    indexed.set_hover(5);
    indexed.acknowledge_change();

    indexed.set_hover(5); // same value
    REQUIRE_FALSE(indexed.changed());
}

// ── Bitwise Flag Helpers ────────────────────────────────────────────

TEST_CASE("ControlStateFlag bitwise OR", "[control][flags]")
{
    auto combined = ControlStateFlag::kHover | ControlStateFlag::kSelected;
    REQUIRE(has_flag(combined, ControlStateFlag::kHover));
    REQUIRE(has_flag(combined, ControlStateFlag::kSelected));
    REQUIRE_FALSE(has_flag(combined, ControlStateFlag::kPressed));
}

TEST_CASE("set_flag and clear_flag", "[control][flags]")
{
    ControlStateFlags flags{0};
    set_flag(flags, ControlStateFlag::kFocused);
    REQUIRE(has_flag(flags, ControlStateFlag::kFocused));

    clear_flag(flags, ControlStateFlag::kFocused);
    REQUIRE_FALSE(has_flag(flags, ControlStateFlag::kFocused));
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
