/// @file test_v23_action_readiness.cpp
/// @brief V23 Phase 02 — Tests for ActionReadinessGate.

#include "core/ActionReadinessGate.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Label tests
// ============================================================================

TEST_CASE("ActionSurfaceKind — labels", "[v23][p02][label]")
{
    CHECK(std::string(action_surface_label(ActionSurfaceKind::kMenu)) == "Menu");
    CHECK(std::string(action_surface_label(ActionSurfaceKind::kToolbar)) == "Toolbar");
    CHECK(std::string(action_surface_label(ActionSurfaceKind::kCommandPalette)) == "CommandPalette");
    CHECK(std::string(action_surface_label(ActionSurfaceKind::kContextMenu)) == "ContextMenu");
    CHECK(std::string(action_surface_label(ActionSurfaceKind::kPanelAction)) == "PanelAction");
}

TEST_CASE("ActionBindingStatus — labels", "[v23][p02][label]")
{
    CHECK(std::string(binding_status_label(ActionBindingStatus::kLive)) == "Live");
    CHECK(std::string(binding_status_label(ActionBindingStatus::kStub)) == "Stub");
    CHECK(std::string(binding_status_label(ActionBindingStatus::kDead)) == "Dead");
    CHECK(std::string(binding_status_label(ActionBindingStatus::kGated)) == "Gated");
    CHECK(std::string(binding_status_label(ActionBindingStatus::kOrphaned)) == "Orphaned");
}

// ============================================================================
// ActionReadinessItem
// ============================================================================

TEST_CASE("ActionReadinessItem — gate blocker logic", "[v23][p02][item]")
{
    ActionReadinessItem item;
    item.action_id = "file.save";

    SECTION("Live is not a blocker") { item.status = ActionBindingStatus::kLive; CHECK(!item.is_gate_blocker()); }
    SECTION("Gated is not a blocker") { item.status = ActionBindingStatus::kGated; CHECK(!item.is_gate_blocker()); }
    SECTION("Dead is a blocker") { item.status = ActionBindingStatus::kDead; CHECK(item.is_gate_blocker()); }
    SECTION("Stub is a blocker") { item.status = ActionBindingStatus::kStub; CHECK(item.is_gate_blocker()); }
    SECTION("Orphaned is a blocker") { item.status = ActionBindingStatus::kOrphaned; CHECK(item.is_gate_blocker()); }
}

// ============================================================================
// PanelReadinessItem
// ============================================================================

TEST_CASE("PanelReadinessItem — gate blocker logic", "[v23][p02][panel]")
{
    PanelReadinessItem panel;
    panel.panel_id = "explorer";

    SECTION("Ready panel is not a blocker")
    {
        panel.has_factory = true;
        panel.is_placeholder = false;
        CHECK(!panel.is_gate_blocker());
    }

    SECTION("Placeholder panel is a blocker")
    {
        panel.has_factory = true;
        panel.is_placeholder = true;
        CHECK(panel.is_gate_blocker());
    }

    SECTION("Missing factory is a blocker")
    {
        panel.has_factory = false;
        CHECK(panel.is_gate_blocker());
    }

    SECTION("Gated placeholder is not a blocker")
    {
        panel.has_factory = true;
        panel.is_placeholder = true;
        panel.is_gated = true;
        CHECK(!panel.is_gate_blocker());
    }
}

// ============================================================================
// Registration and queries
// ============================================================================

TEST_CASE("ActionReadinessGate — action registration", "[v23][p02][gate]")
{
    ActionReadinessGate gate;
    CHECK(gate.action_count() == 0);

    ActionReadinessItem a;
    a.action_id = "file.save";
    a.surface = ActionSurfaceKind::kMenu;
    a.status = ActionBindingStatus::kLive;
    gate.add_action(std::move(a));

    CHECK(gate.action_count() == 1);
}

TEST_CASE("ActionReadinessGate — panel registration", "[v23][p02][gate]")
{
    ActionReadinessGate gate;
    CHECK(gate.panel_count() == 0);

    PanelReadinessItem p;
    p.panel_id = "explorer";
    p.has_factory = true;
    gate.add_panel(std::move(p));

    CHECK(gate.panel_count() == 1);
}

TEST_CASE("ActionReadinessGate — query by surface", "[v23][p02][query]")
{
    ActionReadinessGate gate;

    ActionReadinessItem menu;
    menu.action_id = "file.save";
    menu.surface = ActionSurfaceKind::kMenu;
    gate.add_action(std::move(menu));

    ActionReadinessItem toolbar;
    toolbar.action_id = "file.new";
    toolbar.surface = ActionSurfaceKind::kToolbar;
    gate.add_action(std::move(toolbar));

    CHECK(gate.actions_by_surface(ActionSurfaceKind::kMenu).size() == 1);
    CHECK(gate.actions_by_surface(ActionSurfaceKind::kToolbar).size() == 1);
    CHECK(gate.actions_by_surface(ActionSurfaceKind::kContextMenu).empty());
}

TEST_CASE("ActionReadinessGate — query by status", "[v23][p02][query]")
{
    ActionReadinessGate gate;

    ActionReadinessItem live;
    live.status = ActionBindingStatus::kLive;
    gate.add_action(std::move(live));

    ActionReadinessItem dead;
    dead.status = ActionBindingStatus::kDead;
    gate.add_action(std::move(dead));

    CHECK(gate.actions_by_status(ActionBindingStatus::kLive).size() == 1);
    CHECK(gate.actions_by_status(ActionBindingStatus::kDead).size() == 1);
}

TEST_CASE("ActionReadinessGate — blocking actions", "[v23][p02][query]")
{
    ActionReadinessGate gate;

    ActionReadinessItem live;
    live.status = ActionBindingStatus::kLive;
    gate.add_action(std::move(live));

    ActionReadinessItem stub;
    stub.action_id = "edit.undo";
    stub.status = ActionBindingStatus::kStub;
    gate.add_action(std::move(stub));

    CHECK(gate.blocking_actions().size() == 1);
}

TEST_CASE("ActionReadinessGate — panel queries", "[v23][p02][query]")
{
    ActionReadinessGate gate;

    PanelReadinessItem ready;
    ready.panel_id = "explorer";
    ready.has_factory = true;
    gate.add_panel(std::move(ready));

    PanelReadinessItem placeholder;
    placeholder.panel_id = "search";
    placeholder.is_placeholder = true;
    placeholder.has_factory = true;
    gate.add_panel(std::move(placeholder));

    CHECK(gate.ready_panels().size() == 1);
    CHECK(gate.placeholder_panels().size() == 1);
    CHECK(gate.blocking_panels().size() == 1);
}

// ============================================================================
// Gate check
// ============================================================================

TEST_CASE("ActionReadinessGate — gate passes with all live", "[v23][p02][gate]")
{
    ActionReadinessGate gate;

    ActionReadinessItem live;
    live.status = ActionBindingStatus::kLive;
    gate.add_action(std::move(live));

    PanelReadinessItem ready;
    ready.has_factory = true;
    gate.add_panel(std::move(ready));

    auto result = gate.check_gate();
    CHECK(result.passes);
    CHECK(result.is_clear());
    CHECK(result.live_actions == 1);
    CHECK(result.ready_panels == 1);
}

TEST_CASE("ActionReadinessGate — gate fails with dead actions", "[v23][p02][gate]")
{
    ActionReadinessGate gate;

    ActionReadinessItem dead;
    dead.action_id = "file.save";
    dead.surface = ActionSurfaceKind::kMenu;
    dead.status = ActionBindingStatus::kDead;
    gate.add_action(std::move(dead));

    auto result = gate.check_gate();
    CHECK(!result.passes);
    CHECK(result.dead_actions == 1);
    CHECK(result.action_blocker_count() == 1);
}

TEST_CASE("ActionReadinessGate — gate fails with placeholder panels", "[v23][p02][gate]")
{
    ActionReadinessGate gate;

    PanelReadinessItem placeholder;
    placeholder.panel_id = "pdf_viewer";
    placeholder.has_factory = true;
    placeholder.is_placeholder = true;
    gate.add_panel(std::move(placeholder));

    auto result = gate.check_gate();
    CHECK(!result.passes);
    CHECK(result.placeholder_panels == 1);
    CHECK(result.panel_blocker_count() == 1);
}

TEST_CASE("ActionReadinessGate — clear", "[v23][p02][gate]")
{
    ActionReadinessGate gate;

    ActionReadinessItem a;
    gate.add_action(std::move(a));
    PanelReadinessItem p;
    gate.add_panel(std::move(p));

    gate.clear();
    CHECK(gate.action_count() == 0);
    CHECK(gate.panel_count() == 0);
}

// ============================================================================
// Export
// ============================================================================

TEST_CASE("ActionReadinessGate — JSON export", "[v23][p02][export]")
{
    ActionReadinessGate gate;

    ActionReadinessItem live;
    live.status = ActionBindingStatus::kLive;
    gate.add_action(std::move(live));

    auto json = gate.export_json();
    CHECK(json.find("\"passes\": true") != std::string::npos);
    CHECK(json.find("\"live\": 1") != std::string::npos);
}

TEST_CASE("ActionReadinessGate — Markdown export", "[v23][p02][export]")
{
    ActionReadinessGate gate;

    ActionReadinessItem dead;
    dead.action_id = "broken";
    dead.status = ActionBindingStatus::kDead;
    gate.add_action(std::move(dead));

    auto md = gate.export_markdown();
    CHECK(md.find("FAIL") != std::string::npos);
    CHECK(md.find("Blocking Reasons") != std::string::npos);
}

// ============================================================================
// Integration
// ============================================================================

TEST_CASE("Integration — full action readiness lifecycle", "[v23][p02][integration]")
{
    ActionReadinessGate gate;

    // Mix of live, dead, gated
    ActionReadinessItem live1;
    live1.action_id = "file.save";
    live1.surface = ActionSurfaceKind::kMenu;
    live1.status = ActionBindingStatus::kLive;
    gate.add_action(std::move(live1));

    ActionReadinessItem live2;
    live2.action_id = "file.new";
    live2.surface = ActionSurfaceKind::kToolbar;
    live2.status = ActionBindingStatus::kLive;
    gate.add_action(std::move(live2));

    ActionReadinessItem dead;
    dead.action_id = "git.blame";
    dead.surface = ActionSurfaceKind::kContextMenu;
    dead.status = ActionBindingStatus::kDead;
    gate.add_action(std::move(dead));

    ActionReadinessItem gated;
    gated.action_id = "canvas.collaborate";
    gated.surface = ActionSurfaceKind::kPanelAction;
    gated.status = ActionBindingStatus::kGated;
    gate.add_action(std::move(gated));

    // Panels
    PanelReadinessItem explorer;
    explorer.panel_id = "explorer";
    explorer.has_factory = true;
    gate.add_panel(std::move(explorer));

    PanelReadinessItem search;
    search.panel_id = "search";
    search.has_factory = true;
    search.is_placeholder = true;
    gate.add_panel(std::move(search));

    auto result = gate.check_gate();
    CHECK(!result.passes);
    CHECK(result.total_actions == 4);
    CHECK(result.live_actions == 2);
    CHECK(result.dead_actions == 1);
    CHECK(result.gated_actions == 1);
    CHECK(result.total_panels == 2);
    CHECK(result.ready_panels == 1);
    CHECK(result.placeholder_panels == 1);
    CHECK(result.blocking_reasons.size() == 2); // 1 dead action + 1 placeholder panel
}
