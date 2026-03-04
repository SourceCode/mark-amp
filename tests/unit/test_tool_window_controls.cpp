/**
 * @file test_tool_window_controls.cpp
 * @brief Phase 29: Unit tests for ToolWindowTabControl and PanelLifecycleManager.
 */

#include "core/Events.h"
#include "ui/PanelLifecycleManager.h"
#include "ui/ToolWindowTabControl.h"

#include <catch2/catch_test_macros.hpp>

#include <set>
#include <string>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// ToolWindowTabControl — action definitions
// ═══════════════════════════════════════════════════════

TEST_CASE("ToolWindowTabControl - all action IDs", "[tool_window][tab_control]")
{
    auto ids = ToolWindowTabControl::all_action_ids();
    CHECK(static_cast<int>(ids.size()) == ToolWindowTabControl::action_count());
    CHECK(ids.size() == 15);
}

TEST_CASE("ToolWindowTabControl - action labels", "[tool_window][tab_control]")
{
    auto ids = ToolWindowTabControl::all_action_ids();
    for (const auto& action_id : ids)
    {
        auto label = ToolWindowTabControl::action_label(action_id);
        CHECK_FALSE(label.empty());
    }
}

TEST_CASE("ToolWindowTabControl - action icons", "[tool_window][tab_control]")
{
    auto ids = ToolWindowTabControl::all_action_ids();
    for (const auto& action_id : ids)
    {
        auto icon = ToolWindowTabControl::action_icon(action_id);
        CHECK_FALSE(icon.empty());
    }
}

TEST_CASE("ToolWindowTabControl - action string IDs unique", "[tool_window][tab_control]")
{
    auto ids = ToolWindowTabControl::all_action_ids();
    std::set<std::string> seen;
    for (const auto& action_id : ids)
    {
        auto str_id = ToolWindowTabControl::action_id_to_string(action_id);
        CHECK_FALSE(str_id.empty());
        CHECK(seen.find(str_id) == seen.end());
        seen.insert(str_id);
    }
}

// ═══════════════════════════════════════════════════════
// ToolWindowTabControl — dynamic enablement
// ═══════════════════════════════════════════════════════

TEST_CASE("ToolWindowTabControl - standard actions basic", "[tool_window][tab_control][enable]")
{
    auto actions = ToolWindowTabControl::standard_actions(false, false, 3, 1);
    CHECK(actions.size() > 10);

    // Verify Close is always present and enabled
    bool has_close = false;
    for (const auto& action : actions)
    {
        if (action.action_id == TabActionId::kClose)
        {
            has_close = true;
            CHECK(action.is_enabled);
        }
    }
    CHECK(has_close);
}

TEST_CASE("ToolWindowTabControl - close others disabled with 1 tab",
          "[tool_window][tab_control][enable]")
{
    auto actions = ToolWindowTabControl::standard_actions(false, false, 1, 0);
    for (const auto& action : actions)
    {
        if (action.action_id == TabActionId::kCloseOthers)
        {
            CHECK_FALSE(action.is_enabled);
        }
    }
}

TEST_CASE("ToolWindowTabControl - close right disabled at last tab",
          "[tool_window][tab_control][enable]")
{
    // 3 tabs, at index 2 (last)
    auto actions = ToolWindowTabControl::standard_actions(false, false, 3, 2);
    for (const auto& action : actions)
    {
        if (action.action_id == TabActionId::kCloseRight)
        {
            CHECK_FALSE(action.is_enabled);
        }
    }

    // 3 tabs, at index 1 (not last) — should be enabled
    auto actions2 = ToolWindowTabControl::standard_actions(false, false, 3, 1);
    for (const auto& action : actions2)
    {
        if (action.action_id == TabActionId::kCloseRight)
        {
            CHECK(action.is_enabled);
        }
    }
}

TEST_CASE("ToolWindowTabControl - pin/unpin toggle", "[tool_window][tab_control][enable]")
{
    SECTION("Not pinned — shows Pin")
    {
        auto actions = ToolWindowTabControl::standard_actions(false, false, 3, 0);
        bool has_pin = false;
        bool has_unpin = false;
        for (const auto& action : actions)
        {
            if (action.action_id == TabActionId::kPin)
            {
                has_pin = true;
            }
            if (action.action_id == TabActionId::kUnpin)
            {
                has_unpin = true;
            }
        }
        CHECK(has_pin);
        CHECK_FALSE(has_unpin);
    }

    SECTION("Pinned — shows Unpin")
    {
        auto actions = ToolWindowTabControl::standard_actions(true, false, 3, 0);
        bool has_pin = false;
        bool has_unpin = false;
        for (const auto& action : actions)
        {
            if (action.action_id == TabActionId::kPin)
            {
                has_pin = true;
            }
            if (action.action_id == TabActionId::kUnpin)
            {
                has_unpin = true;
            }
        }
        CHECK_FALSE(has_pin);
        CHECK(has_unpin);
    }
}

TEST_CASE("ToolWindowTabControl - maximize/restore toggle", "[tool_window][tab_control][enable]")
{
    SECTION("Not maximized — shows Maximize")
    {
        auto actions = ToolWindowTabControl::standard_actions(false, false, 3, 0);
        bool has_maximize = false;
        bool has_restore = false;
        for (const auto& action : actions)
        {
            if (action.action_id == TabActionId::kMaximize)
            {
                has_maximize = true;
            }
            if (action.action_id == TabActionId::kRestore)
            {
                has_restore = true;
            }
        }
        CHECK(has_maximize);
        CHECK_FALSE(has_restore);
    }

    SECTION("Maximized — shows Restore")
    {
        auto actions = ToolWindowTabControl::standard_actions(false, true, 3, 0);
        bool has_maximize = false;
        bool has_restore = false;
        for (const auto& action : actions)
        {
            if (action.action_id == TabActionId::kMaximize)
            {
                has_maximize = true;
            }
            if (action.action_id == TabActionId::kRestore)
            {
                has_restore = true;
            }
        }
        CHECK_FALSE(has_maximize);
        CHECK(has_restore);
    }
}

// ═══════════════════════════════════════════════════════
// PanelLifecycleManager — snapshot CRUD
// ═══════════════════════════════════════════════════════

TEST_CASE("PanelLifecycleManager - empty state", "[tool_window][lifecycle]")
{
    PanelLifecycleManager mgr;
    CHECK(mgr.snapshot_count() == 0);
    CHECK(mgr.snapshot_names().empty());
    CHECK_FALSE(mgr.has_snapshot("anything"));
    CHECK(mgr.get_snapshot("anything") == nullptr);
    CHECK(mgr.default_layout() == nullptr);
}

TEST_CASE("PanelLifecycleManager - save and get snapshot", "[tool_window][lifecycle]")
{
    PanelLifecycleManager mgr;
    auto defaults = PanelLifecycleManager::builtin_defaults();

    mgr.save_snapshot("layout_a", defaults);
    CHECK(mgr.snapshot_count() == 1);
    CHECK(mgr.has_snapshot("layout_a"));

    const auto* snap = mgr.get_snapshot("layout_a");
    REQUIRE(snap != nullptr);
    CHECK(snap->snapshot_name == "layout_a");
    CHECK(snap->entries.size() == defaults.size());
}

TEST_CASE("PanelLifecycleManager - overwrite snapshot", "[tool_window][lifecycle]")
{
    PanelLifecycleManager mgr;
    auto defaults = PanelLifecycleManager::builtin_defaults();

    mgr.save_snapshot("layout_a", defaults);
    CHECK(mgr.snapshot_count() == 1);

    // Overwrite with fewer entries
    std::vector<PanelLayoutEntry> fewer = {defaults[0], defaults[1]};
    mgr.save_snapshot("layout_a", fewer);
    CHECK(mgr.snapshot_count() == 1); // Still 1, not 2

    const auto* snap = mgr.get_snapshot("layout_a");
    REQUIRE(snap != nullptr);
    CHECK(snap->entries.size() == 2);
}

TEST_CASE("PanelLifecycleManager - delete snapshot", "[tool_window][lifecycle]")
{
    PanelLifecycleManager mgr;
    auto defaults = PanelLifecycleManager::builtin_defaults();

    mgr.save_snapshot("layout_a", defaults);
    mgr.save_snapshot("layout_b", defaults);
    CHECK(mgr.snapshot_count() == 2);

    mgr.delete_snapshot("layout_a");
    CHECK(mgr.snapshot_count() == 1);
    CHECK_FALSE(mgr.has_snapshot("layout_a"));
    CHECK(mgr.has_snapshot("layout_b"));
}

TEST_CASE("PanelLifecycleManager - snapshot names sorted", "[tool_window][lifecycle]")
{
    PanelLifecycleManager mgr;
    auto defaults = PanelLifecycleManager::builtin_defaults();

    mgr.save_snapshot("zen_mode", defaults);
    mgr.save_snapshot("debug_layout", defaults);
    mgr.save_snapshot("coding_layout", defaults);

    auto names = mgr.snapshot_names();
    REQUIRE(names.size() == 3);
    CHECK(names[0] == "coding_layout");
    CHECK(names[1] == "debug_layout");
    CHECK(names[2] == "zen_mode");
}

TEST_CASE("PanelLifecycleManager - default layout", "[tool_window][lifecycle]")
{
    PanelLifecycleManager mgr;
    auto defaults = PanelLifecycleManager::builtin_defaults();

    CHECK(mgr.default_layout() == nullptr);

    mgr.set_default_layout(defaults);
    const auto* def = mgr.default_layout();
    REQUIRE(def != nullptr);
    CHECK(def->entries.size() == defaults.size());
}

TEST_CASE("PanelLifecycleManager - clear", "[tool_window][lifecycle]")
{
    PanelLifecycleManager mgr;
    auto defaults = PanelLifecycleManager::builtin_defaults();

    mgr.save_snapshot("test", defaults);
    mgr.set_default_layout(defaults);
    CHECK(mgr.snapshot_count() == 1);
    CHECK(mgr.default_layout() != nullptr);

    mgr.clear();
    CHECK(mgr.snapshot_count() == 0);
    CHECK(mgr.default_layout() == nullptr);
}

// ═══════════════════════════════════════════════════════
// PanelSnapshot — queries
// ═══════════════════════════════════════════════════════

TEST_CASE("PanelSnapshot - visible count", "[tool_window][lifecycle][snapshot]")
{
    auto defaults = PanelLifecycleManager::builtin_defaults();
    PanelSnapshot snap;
    snap.entries = defaults;

    // Builtin defaults: explorer visible, terminal visible = 2
    CHECK(snap.visible_count() == 2);
}

TEST_CASE("PanelSnapshot - count at position", "[tool_window][lifecycle][snapshot]")
{
    auto defaults = PanelLifecycleManager::builtin_defaults();
    PanelSnapshot snap;
    snap.entries = defaults;

    // Left: explorer, search = 2; Bottom: output, problems, terminal, build, debug = 5
    CHECK(snap.count_at(markamp::core::events::DockPosition::kLeft) == 2);
    CHECK(snap.count_at(markamp::core::events::DockPosition::kBottom) == 5);
}

TEST_CASE("PanelSnapshot - is empty", "[tool_window][lifecycle][snapshot]")
{
    PanelSnapshot snap;
    CHECK(snap.is_empty());

    snap.entries = PanelLifecycleManager::builtin_defaults();
    CHECK_FALSE(snap.is_empty());
}

// ═══════════════════════════════════════════════════════
// Builtin defaults validation
// ═══════════════════════════════════════════════════════

TEST_CASE("Builtin defaults have 7 panels", "[tool_window][lifecycle][defaults]")
{
    auto defaults = PanelLifecycleManager::builtin_defaults();
    CHECK(defaults.size() == 7);

    // Verify all panel IDs are unique
    std::set<std::string> ids;
    for (const auto& entry : defaults)
    {
        CHECK(ids.find(entry.panel_id) == ids.end());
        ids.insert(entry.panel_id);
    }
}

TEST_CASE("Builtin defaults have titles", "[tool_window][lifecycle][defaults]")
{
    auto defaults = PanelLifecycleManager::builtin_defaults();
    for (const auto& entry : defaults)
    {
        INFO("Panel: " << entry.panel_id);
        CHECK_FALSE(entry.title.empty());
    }
}
