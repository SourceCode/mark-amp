/// @file test_tool_window_host.cpp
/// @brief Phase 18 — Comprehensive unit tests and benchmarks for ToolWindowHost.

#include "core/Config.h"
#include "core/EventBus.h"
#include "ui/ToolWindowHost.h"

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp;
using namespace markamp::core;
using namespace markamp::core::events;
using namespace markamp::ui;

// ── Test fixture ────────────────────────────────────────────────────

struct TestFixture
{
    EventBus bus;
    Config config;
    ToolWindowHost host{bus, config};
};

// ── Task 16: Comprehensive Unit Tests ───────────────────────────────

TEST_CASE("ToolWindowHost register and unregister", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");
    fix.host.register_panel("problems", "Problems");

    REQUIRE(fix.host.panel_count() == 2);

    auto ids = fix.host.panel_ids();
    REQUIRE(ids.size() == 2);

    fix.host.unregister_panel("output");
    REQUIRE(fix.host.panel_count() == 1);

    // Duplicate registration is a no-op
    fix.host.register_panel("problems", "Problems Again");
    REQUIRE(fix.host.panel_count() == 1);
}

TEST_CASE("ToolWindowHost toggle visibility", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");
    REQUIRE_FALSE(fix.host.is_visible("output"));

    fix.host.toggle_panel("output");
    REQUIRE(fix.host.is_visible("output"));

    fix.host.toggle_panel("output");
    REQUIRE_FALSE(fix.host.is_visible("output"));

    fix.host.set_visible("output", true);
    REQUIRE(fix.host.is_visible("output"));
}

TEST_CASE("ToolWindowHost pin and unpin", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");
    REQUIRE_FALSE(fix.host.is_pinned("output"));

    fix.host.set_pinned("output", true);
    REQUIRE(fix.host.is_pinned("output"));

    fix.host.set_pinned("output", false);
    REQUIRE_FALSE(fix.host.is_pinned("output"));
}

TEST_CASE("ToolWindowHost dock position", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output", DockPosition::kBottom);
    REQUIRE(fix.host.dock_position("output") == DockPosition::kBottom);

    fix.host.set_dock_position("output", DockPosition::kLeft);
    REQUIRE(fix.host.dock_position("output") == DockPosition::kLeft);
}

// Task 1: Dock constraints
TEST_CASE("ToolWindowHost dock constraints", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output", DockPosition::kBottom);

    // Restrict to bottom only
    fix.host.set_allowed_positions("output", {DockPosition::kBottom});

    // Try to dock left — should be rejected
    fix.host.set_dock_position("output", DockPosition::kLeft);
    REQUIRE(fix.host.dock_position("output") == DockPosition::kBottom);

    // Bottom should still work
    fix.host.set_dock_position("output", DockPosition::kBottom);
    REQUIRE(fix.host.dock_position("output") == DockPosition::kBottom);

    REQUIRE(fix.host.is_position_allowed("output", DockPosition::kBottom));
    REQUIRE_FALSE(fix.host.is_position_allowed("output", DockPosition::kLeft));
}

TEST_CASE("ToolWindowHost dock constraints auto-relocate", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output", DockPosition::kLeft);

    // Set allowed positions that exclude current position
    fix.host.set_allowed_positions("output", {DockPosition::kBottom});

    // Panel should have been moved to an allowed position
    REQUIRE(fix.host.dock_position("output") == DockPosition::kBottom);
}

// Task 2: Auto-hide
TEST_CASE("ToolWindowHost auto-hide", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");
    REQUIRE_FALSE(fix.host.is_auto_hide("output"));

    fix.host.set_auto_hide("output", true);
    REQUIRE(fix.host.is_auto_hide("output"));

    REQUIRE(fix.host.auto_hide_timeout("output") == 2000);

    fix.host.set_auto_hide_timeout("output", 500);
    REQUIRE(fix.host.auto_hide_timeout("output") == 500);

    // Minimum timeout enforcement
    fix.host.set_auto_hide_timeout("output", 10);
    REQUIRE(fix.host.auto_hide_timeout("output") == 100);
}

// Task 3: Size constraints
TEST_CASE("ToolWindowHost size constraints", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");

    auto constraint = fix.host.size_constraint("output");
    REQUIRE(constraint.min_width == 100);
    REQUIRE(constraint.max_width == 2000);

    fix.host.set_min_size("output", 200, 150);
    constraint = fix.host.size_constraint("output");
    REQUIRE(constraint.min_width == 200);
    REQUIRE(constraint.min_height == 150);

    const auto* state = fix.host.panel_state("output");
    REQUIRE(state != nullptr);
    REQUIRE(state->width >= 200);
    REQUIRE(state->height >= 150);

    fix.host.set_max_size("output", 250, 180);
    state = fix.host.panel_state("output");
    REQUIRE(state->width <= 250);
    REQUIRE(state->height <= 180);
}

// Task 4: Ordering
TEST_CASE("ToolWindowHost ordering", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("a", "A", DockPosition::kBottom);
    fix.host.register_panel("b", "B", DockPosition::kBottom);
    fix.host.register_panel("c", "C", DockPosition::kBottom);

    auto ordered = fix.host.panels_at(DockPosition::kBottom);
    REQUIRE(ordered.size() == 3);
    REQUIRE(ordered[0] == "a");
    REQUIRE(ordered[1] == "b");
    REQUIRE(ordered[2] == "c");

    // Move c before a
    fix.host.move_panel_before("c", "a");
    ordered = fix.host.panels_at(DockPosition::kBottom);
    REQUIRE(ordered[0] == "c");

    // Set explicit order
    fix.host.set_order("a", 10);
    fix.host.set_order("b", 20);
    fix.host.set_order("c", 30);
    ordered = fix.host.panels_at(DockPosition::kBottom);
    REQUIRE(ordered[0] == "a");
    REQUIRE(ordered[1] == "b");
    REQUIRE(ordered[2] == "c");
}

// Task 5: State serialization
TEST_CASE("ToolWindowHost serialization round-trip", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output", DockPosition::kBottom);
    fix.host.set_visible("output", true);
    fix.host.set_pinned("output", true);
    fix.host.set_auto_hide("output", true);
    fix.host.set_auto_hide_timeout("output", 3000);
    fix.host.set_order("output", 42);
    fix.host.set_min_size("output", 200, 150);

    auto json = fix.host.save_layout();
    REQUIRE_FALSE(json.empty());
    REQUIRE(json.find("output") != std::string::npos);
    REQUIRE(json.find("\"pinned\":true") != std::string::npos);

    // Create a fresh host and restore
    TestFixture fix2;
    fix2.host.register_panel("output", "Output");
    REQUIRE(fix2.host.restore_layout(json));

    REQUIRE(fix2.host.is_visible("output"));
    REQUIRE(fix2.host.is_pinned("output"));
    REQUIRE(fix2.host.is_auto_hide("output"));
    REQUIRE(fix2.host.auto_hide_timeout("output") == 3000);
    REQUIRE(fix2.host.order("output") == 42);
}

// Task 6: Panel commands
TEST_CASE("ToolWindowHost commands", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");
    fix.host.register_command("output", "clear");
    fix.host.register_command("output", "export");

    auto cmds = fix.host.commands("output");
    REQUIRE(cmds.size() == 2);

    // Duplicate registration is a no-op
    fix.host.register_command("output", "clear");
    cmds = fix.host.commands("output");
    REQUIRE(cmds.size() == 2);

    fix.host.unregister_command("output", "clear");
    cmds = fix.host.commands("output");
    REQUIRE(cmds.size() == 1);
    REQUIRE(cmds[0] == "export");

    // Execute dispatches event
    bool dispatched = false;
    auto sub = fix.bus.subscribe<PanelCommandEvent>(
        [&](const PanelCommandEvent& evt)
        {
            dispatched = true;
            REQUIRE(evt.panel_id == "output");
            REQUIRE(evt.command_id == "export");
        });
    fix.host.execute_command("output", "export");
    REQUIRE(dispatched);
}

// Task 7: Keyboard navigation
TEST_CASE("ToolWindowHost keyboard navigation", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("a", "A", DockPosition::kBottom);
    fix.host.register_panel("b", "B", DockPosition::kBottom);
    fix.host.register_panel("c", "C", DockPosition::kBottom);
    fix.host.set_visible("a", true);
    fix.host.set_visible("b", true);
    fix.host.set_visible("c", true);

    REQUIRE(fix.host.focused_panel().empty());

    fix.host.focus_panel("a");
    REQUIRE(fix.host.focused_panel() == "a");

    bool focus_received = false;
    auto sub = fix.bus.subscribe<PanelFocusChangedEvent>(
        [&](const PanelFocusChangedEvent& evt)
        {
            focus_received = true;
            REQUIRE(evt.focused);
        });

    fix.host.focus_next_panel();
    REQUIRE(focus_received);
}

// Task 8: Badges
TEST_CASE("ToolWindowHost badges", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("problems", "Problems");

    PanelBadge badge_val;
    badge_val.count = 5;
    badge_val.text = "5 errors";

    bool badge_event = false;
    std::string received_panel_id;
    int received_count = 0;
    auto sub = fix.bus.subscribe<PanelBadgeChangedEvent>(
        [&](const PanelBadgeChangedEvent& evt)
        {
            badge_event = true;
            received_panel_id = evt.panel_id;
            received_count = evt.badge.count;
        });

    fix.host.set_badge("problems", badge_val);
    REQUIRE(badge_event);
    REQUIRE(received_panel_id == "problems");
    REQUIRE(received_count == 5);
    REQUIRE(fix.host.badge("problems").count == 5);

    fix.host.clear_badge("problems");
    REQUIRE(fix.host.badge("problems").count == 0);
}

// Task 9: Context menus
TEST_CASE("ToolWindowHost context menus", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");

    std::vector<PanelMenuItem> items;
    items.push_back({"Clear", "clear", true, false});
    items.push_back({"", "", false, true}); // separator
    items.push_back({"Export", "export", true, false});

    fix.host.set_context_menu("output", items);
    auto menu = fix.host.context_menu("output");
    REQUIRE(menu.size() == 3);
    REQUIRE(menu[0].label == "Clear");
    REQUIRE(menu[1].separator);
    REQUIRE(menu[2].command_id == "export");
}

// Task 10: Grouping
TEST_CASE("ToolWindowHost grouping", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");
    fix.host.register_panel("problems", "Problems");
    fix.host.register_panel("terminal", "Terminal");

    fix.host.set_group("output", "bottom");
    fix.host.set_group("problems", "bottom");
    fix.host.set_group("terminal", "bottom");

    REQUIRE(fix.host.group("output") == "bottom");

    auto bottom_panels = fix.host.panels_in_group("bottom");
    REQUIRE(bottom_panels.size() == 3);
}

// Task 11: History
TEST_CASE("ToolWindowHost history", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");
    fix.host.register_panel("problems", "Problems");

    fix.host.set_visible("output", true);
    fix.host.set_visible("problems", true);

    // Hiding adds to history
    fix.host.set_visible("output", false);
    REQUIRE(fix.host.panel_history().size() == 1);

    fix.host.set_visible("problems", false);
    REQUIRE(fix.host.panel_history().size() == 2);

    // show_last_panel re-shows the most recently hidden
    fix.host.show_last_panel();
    REQUIRE(fix.host.is_visible("problems"));

    fix.host.show_last_panel();
    REQUIRE(fix.host.is_visible("output"));
}

// Task 12: Zoom
TEST_CASE("ToolWindowHost zoom", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");

    REQUIRE(fix.host.zoom_level("output") == 1.0);

    fix.host.zoom_in("output");
    REQUIRE(fix.host.zoom_level("output") > 1.0);

    fix.host.zoom_out("output");
    fix.host.zoom_out("output");
    REQUIRE(fix.host.zoom_level("output") < 1.0);

    fix.host.reset_zoom("output");
    REQUIRE(fix.host.zoom_level("output") == 1.0);

    // Clamping
    fix.host.set_zoom_level("output", 100.0);
    REQUIRE(fix.host.zoom_level("output") <= 5.0);

    fix.host.set_zoom_level("output", -1.0);
    REQUIRE(fix.host.zoom_level("output") >= 0.1);
}

// Task 13: Search
TEST_CASE("ToolWindowHost search", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");

    REQUIRE_FALSE(fix.host.is_searchable("output"));

    fix.host.set_searchable("output", true);
    REQUIRE(fix.host.is_searchable("output"));

    bool search_event = false;
    std::string received_panel_id;
    std::string received_text;
    auto sub = fix.bus.subscribe<PanelSearchChangedEvent>(
        [&](const PanelSearchChangedEvent& evt)
        {
            search_event = true;
            received_panel_id = evt.panel_id;
            received_text = evt.search_text;
        });

    fix.host.set_search_text("output", "error");
    REQUIRE(search_event);
    REQUIRE(received_panel_id == "output");
    REQUIRE(received_text == "error");
    REQUIRE(fix.host.search_text("output") == "error");

    fix.host.clear_search("output");
    REQUIRE(fix.host.search_text("output").empty());
}

// Task 14: Theme integration
TEST_CASE("ToolWindowHost theme integration", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");

    bool theme_applied = false;
    fix.host.set_theme_callback("output",
                                [&](const std::string& panel_id)
                                {
                                    theme_applied = true;
                                    REQUIRE(panel_id == "output");
                                });

    fix.host.apply_theme_to_all();
    REQUIRE(theme_applied);
}

// Task 15: Extension support
TEST_CASE("ToolWindowHost extension panels", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_extension_panel("ext1", "ext1.panel1", "Ext1 Panel 1");
    fix.host.register_extension_panel("ext1", "ext1.panel2", "Ext1 Panel 2");
    fix.host.register_extension_panel("ext2", "ext2.panel1", "Ext2 Panel 1");

    REQUIRE(fix.host.panel_count() == 3);

    auto ext1_panels = fix.host.extension_panels("ext1");
    REQUIRE(ext1_panels.size() == 2);

    // Bulk unregister
    fix.host.unregister_extension_panels("ext1");
    REQUIRE(fix.host.panel_count() == 1);
    REQUIRE(fix.host.extension_panels("ext1").empty());

    // Ext2 still exists
    REQUIRE(fix.host.extension_panels("ext2").size() == 1);
}

// Task 16: Panel state query
TEST_CASE("ToolWindowHost panel state query", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output", DockPosition::kBottom);

    const auto* state = fix.host.panel_state("output");
    REQUIRE(state != nullptr);
    REQUIRE(state->panel_id == "output");
    REQUIRE(state->title == "Output");
    REQUIRE(state->dock_position == DockPosition::kBottom);

    // Non-existent panel
    REQUIRE(fix.host.panel_state("nonexistent") == nullptr);
}

// Task 16: Event-driven toggle
TEST_CASE("ToolWindowHost event-driven toggle", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output");

    ToolWindowToggleRequestEvent evt;
    evt.panel_id = "output";
    evt.visible = true;
    fix.bus.publish(evt);

    REQUIRE(fix.host.is_visible("output"));
}

// Task 16: Event-driven dock change
TEST_CASE("ToolWindowHost event-driven dock change", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output", DockPosition::kBottom);

    ToolWindowDockPositionChangedEvent evt;
    evt.panel_id = "output";
    evt.dock_position = DockPosition::kRight;
    fix.bus.publish(evt);

    REQUIRE(fix.host.dock_position("output") == DockPosition::kRight);
}

// Task 16: Edge cases
TEST_CASE("ToolWindowHost edge cases", "[tool_window_host]")
{
    TestFixture fix;

    // Operations on non-existent panels should be no-ops
    fix.host.toggle_panel("nonexistent");
    fix.host.set_visible("nonexistent", true);
    fix.host.set_dock_position("nonexistent", DockPosition::kLeft);
    fix.host.set_pinned("nonexistent", true);
    fix.host.set_auto_hide("nonexistent", true);
    fix.host.set_min_size("nonexistent", 100, 100);
    fix.host.set_max_size("nonexistent", 500, 500);
    fix.host.set_order("nonexistent", 5);
    fix.host.set_badge("nonexistent", {});
    fix.host.focus_panel("nonexistent");
    fix.host.set_searchable("nonexistent", true);
    fix.host.set_zoom_level("nonexistent", 2.0);
    fix.host.set_group("nonexistent", "test");

    REQUIRE_FALSE(fix.host.is_visible("nonexistent"));
    REQUIRE_FALSE(fix.host.is_pinned("nonexistent"));
    REQUIRE_FALSE(fix.host.is_auto_hide("nonexistent"));
    REQUIRE(fix.host.panel_count() == 0);
}

// Task 16: Empty serialization
TEST_CASE("ToolWindowHost empty serialization", "[tool_window_host]")
{
    TestFixture fix;

    auto json = fix.host.save_layout();
    REQUIRE_FALSE(json.empty());

    REQUIRE(fix.host.restore_layout(""));
    REQUIRE(fix.host.restore_layout("{}"));
}

// ── Task 17: Performance Benchmarks ─────────────────────────────────

TEST_CASE("ToolWindowHost benchmark: registration", "[tool_window_host][!benchmark]")
{
    TestFixture fix;

    BENCHMARK("register 1000 panels")
    {
        for (int idx = 0; idx < 1000; ++idx)
        {
            fix.host.register_panel("bench_" + std::to_string(idx), "Bench " + std::to_string(idx));
        }
        for (int idx = 0; idx < 1000; ++idx)
        {
            fix.host.unregister_panel("bench_" + std::to_string(idx));
        }
    };
}

TEST_CASE("ToolWindowHost benchmark: toggle latency", "[tool_window_host][!benchmark]")
{
    TestFixture fix;
    fix.host.register_panel("bench", "Bench");

    BENCHMARK("toggle 10000 times")
    {
        for (int idx = 0; idx < 10000; ++idx)
        {
            fix.host.toggle_panel("bench");
        }
    };
}

TEST_CASE("ToolWindowHost benchmark: serialization", "[tool_window_host][!benchmark]")
{
    TestFixture fix;
    for (int idx = 0; idx < 50; ++idx)
    {
        fix.host.register_panel("panel_" + std::to_string(idx), "Panel " + std::to_string(idx));
    }

    BENCHMARK("save_layout 50 panels")
    {
        return fix.host.save_layout();
    };
}

TEST_CASE("ToolWindowHost benchmark: panel lookup", "[tool_window_host][!benchmark]")
{
    TestFixture fix;
    for (int idx = 0; idx < 100; ++idx)
    {
        fix.host.register_panel("panel_" + std::to_string(idx), "Panel " + std::to_string(idx));
    }

    BENCHMARK("lookup by ID 10000 times")
    {
        for (int idx = 0; idx < 10000; ++idx)
        {
            static_cast<void>(fix.host.panel_state("panel_50"));
        }
    };
}

// ── Task 18: Accessibility labels ───────────────────────────────────

TEST_CASE("ToolWindowHost accessible names", "[tool_window_host]")
{
    TestFixture fix;

    fix.host.register_panel("output", "Output Panel");

    const auto* state = fix.host.panel_state("output");
    REQUIRE(state != nullptr);
    // Panel title serves as the accessible name
    REQUIRE(state->title == "Output Panel");
}
