/// @file test_floating_toolbars.cpp
/// @brief V13 Phase 30 — Unit tests for the floating toolbar system.

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"
#include "ui/FloatingToolbar.h"
#include "ui/FloatingToolbarManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

// ─────────────────────────────────────────────────────────────────────────────
// FloatingToolbarButton
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("FloatingToolbarButton default state", "[floating_toolbar]")
{
    FloatingToolbarButton btn;
    REQUIRE(btn.id.empty());
    REQUIRE(btn.label.empty());
    REQUIRE(btn.tooltip.empty());
    REQUIRE(btn.shortcut.empty());
    REQUIRE_FALSE(btn.callback);
    REQUIRE_FALSE(btn.is_enabled);
    REQUIRE_FALSE(btn.is_toggled);
    REQUIRE_FALSE(btn.is_toggle);
    REQUIRE_FALSE(btn.is_separator);
}

TEST_CASE("FloatingToolbarButton separator flag", "[floating_toolbar]")
{
    FloatingToolbarButton sep;
    sep.is_separator = true;
    REQUIRE(sep.is_separator);
    REQUIRE(sep.id.empty());
}

TEST_CASE("FloatingToolbarButton with enabled callback", "[floating_toolbar]")
{
    FloatingToolbarButton btn;
    btn.id = "test_btn";
    btn.label = "T";
    btn.tooltip = "Test Button";

    bool enabled = false;
    btn.is_enabled = [&enabled]() { return enabled; };

    REQUIRE_FALSE(btn.is_enabled());
    enabled = true;
    REQUIRE(btn.is_enabled());
}

TEST_CASE("FloatingToolbarButton toggle callback", "[floating_toolbar]")
{
    FloatingToolbarButton btn;
    btn.is_toggle = true;

    bool toggled = false;
    btn.is_toggled = [&toggled]() { return toggled; };

    REQUIRE_FALSE(btn.is_toggled());
    toggled = true;
    REQUIRE(btn.is_toggled());
}

TEST_CASE("FloatingToolbarButton callback invocation", "[floating_toolbar]")
{
    FloatingToolbarButton btn;
    int call_count = 0;
    btn.callback = [&call_count]() { ++call_count; };

    btn.callback();
    btn.callback();
    REQUIRE(call_count == 2);
}

// ─────────────────────────────────────────────────────────────────────────────
// FloatingToolbarEdge
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("FloatingToolbarEdge enum values", "[floating_toolbar]")
{
    REQUIRE(static_cast<uint8_t>(FloatingToolbarEdge::kNone) == 0);
    REQUIRE(static_cast<uint8_t>(FloatingToolbarEdge::kTop) == 1);
    REQUIRE(static_cast<uint8_t>(FloatingToolbarEdge::kBottom) == 2);
    REQUIRE(static_cast<uint8_t>(FloatingToolbarEdge::kLeft) == 3);
    REQUIRE(static_cast<uint8_t>(FloatingToolbarEdge::kRight) == 4);
}

// ─────────────────────────────────────────────────────────────────────────────
// FloatingToolbar constants
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("FloatingToolbar constants", "[floating_toolbar]")
{
    REQUIRE(FloatingToolbar::kButtonSize == 28);
    REQUIRE(FloatingToolbar::kButtonPadding == 2);
    REQUIRE(FloatingToolbar::kDragHandleWidth == 12);
    REQUIRE(FloatingToolbar::kBorderRadius == 8);
    REQUIRE(FloatingToolbar::kShadowOffset == 3);
    REQUIRE(FloatingToolbar::kSnapDistance == 20);
    REQUIRE(FloatingToolbar::kDefaultAutoHideMs == 5000);
}

// ─────────────────────────────────────────────────────────────────────────────
// FloatingToolbarManager (non-GUI logic)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("FloatingToolbarManager initial state", "[floating_toolbar_manager]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine theme(bus, registry);
    FloatingToolbarManager mgr(nullptr, theme, bus);

    REQUIRE(mgr.toolbar_count() == 0);
    REQUIRE(mgr.visible_count() == 0);
    REQUIRE(mgr.z_order().empty());
}

TEST_CASE("FloatingToolbarManager GetToolbar returns nullptr for unknown id",
          "[floating_toolbar_manager]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine theme(bus, registry);
    FloatingToolbarManager mgr(nullptr, theme, bus);

    REQUIRE(mgr.GetToolbar("nonexistent") == nullptr);
}

TEST_CASE("FloatingToolbarManager IsVisible returns false for unknown id",
          "[floating_toolbar_manager]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine theme(bus, registry);
    FloatingToolbarManager mgr(nullptr, theme, bus);

    REQUIRE_FALSE(mgr.IsVisible("nonexistent"));
}

TEST_CASE("FloatingToolbarManager z_order BringToFront ordering", "[floating_toolbar_manager]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine theme(bus, registry);
    FloatingToolbarManager mgr(nullptr, theme, bus);

    // Bring to front inserts at position 0
    mgr.BringToFront("toolbar_a");
    mgr.BringToFront("toolbar_b");
    mgr.BringToFront("toolbar_c");

    const auto& order = mgr.z_order();
    REQUIRE(order.size() == 3);
    REQUIRE(order[0] == "toolbar_c"); // most recent = front
    REQUIRE(order[1] == "toolbar_b");
    REQUIRE(order[2] == "toolbar_a");

    // Bring "a" back to front
    mgr.BringToFront("toolbar_a");
    REQUIRE(order[0] == "toolbar_a");
    REQUIRE(order[1] == "toolbar_c");
    REQUIRE(order[2] == "toolbar_b");
}

TEST_CASE("FloatingToolbarManager HideAll on empty is safe", "[floating_toolbar_manager]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine theme(bus, registry);
    FloatingToolbarManager mgr(nullptr, theme, bus);

    REQUIRE_NOTHROW(mgr.HideAll());
}

TEST_CASE("FloatingToolbarManager SavePositions on empty is safe", "[floating_toolbar_manager]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine theme(bus, registry);
    FloatingToolbarManager mgr(nullptr, theme, bus);

    REQUIRE_NOTHROW(mgr.SavePositions());
    REQUIRE_NOTHROW(mgr.RestorePositions());
}

// ─────────────────────────────────────────────────────────────────────────────
// Phase 30 Events
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("FloatingToolbarShownEvent fields", "[floating_toolbar_events]")
{
    events::FloatingToolbarShownEvent evt;
    evt.toolbar_id = "debug_toolbar";
    REQUIRE(evt.toolbar_id == "debug_toolbar");
}

TEST_CASE("FloatingToolbarHiddenEvent fields", "[floating_toolbar_events]")
{
    events::FloatingToolbarHiddenEvent evt;
    evt.toolbar_id = "format_bar";
    REQUIRE(evt.toolbar_id == "format_bar");
}

TEST_CASE("FloatingToolbarMovedEvent fields", "[floating_toolbar_events]")
{
    events::FloatingToolbarMovedEvent evt;
    evt.toolbar_id = "debug_toolbar";
    evt.pos_x = 100;
    evt.pos_y = 200;
    REQUIRE(evt.toolbar_id == "debug_toolbar");
    REQUIRE(evt.pos_x == 100);
    REQUIRE(evt.pos_y == 200);
}

TEST_CASE("FloatingToolbarMovedEvent defaults", "[floating_toolbar_events]")
{
    events::FloatingToolbarMovedEvent evt;
    REQUIRE(evt.pos_x == 0);
    REQUIRE(evt.pos_y == 0);
    REQUIRE(evt.toolbar_id.empty());
}

TEST_CASE("Event publish/subscribe roundtrip for FloatingToolbarShownEvent",
          "[floating_toolbar_events]")
{
    EventBus bus;
    std::string received_id;

    auto sub = bus.subscribe<events::FloatingToolbarShownEvent>(
        [&received_id](const events::FloatingToolbarShownEvent& evt)
        { received_id = evt.toolbar_id; });

    events::FloatingToolbarShownEvent evt;
    evt.toolbar_id = "selection_bar";
    bus.publish(evt);

    REQUIRE(received_id == "selection_bar");
}

TEST_CASE("Event publish/subscribe roundtrip for FloatingToolbarMovedEvent",
          "[floating_toolbar_events]")
{
    EventBus bus;
    int received_x = -1;
    int received_y = -1;

    auto sub = bus.subscribe<events::FloatingToolbarMovedEvent>(
        [&received_x, &received_y](const events::FloatingToolbarMovedEvent& evt)
        {
            received_x = evt.pos_x;
            received_y = evt.pos_y;
        });

    events::FloatingToolbarMovedEvent evt;
    evt.toolbar_id = "debug_toolbar";
    evt.pos_x = 42;
    evt.pos_y = 84;
    bus.publish(evt);

    REQUIRE(received_x == 42);
    REQUIRE(received_y == 84);
}
