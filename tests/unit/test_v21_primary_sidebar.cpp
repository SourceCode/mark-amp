/// @file test_v21_primary_sidebar.cpp
/// @brief V21 Phase 05 — Tests for Panel Registry, Primary Sidebar & Controlled Surface Replacement.
///
/// Tests cover:
///   - PanelLifecycleAuditor: registration, lifecycle states, readiness filtering,
///     placeholder detection, header actions, explorer sections, diagnostics
///   - SidebarStateContract: panel state tracking, snapshots, named snapshots,
///     restore operations
///   - Integration: lifecycle auditor + state contract + manifest lifecycle

#include "core/ControlActionManifest.h"
#include "core/PanelLifecycleAuditor.h"
#include "core/SidebarStateContract.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Helper: create a panel-oriented manifest
// ============================================================================

static auto make_panel_manifest() -> ControlActionManifest
{
    ControlActionManifest manifest;

    ActionEntry collapse;
    collapse.action_id = "panel.collapseAll";
    collapse.label = "Collapse All";
    collapse.handler = []() -> bool { return true; };
    collapse.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(collapse));

    ActionEntry refresh;
    refresh.action_id = "explorer.refresh";
    refresh.label = "Refresh Explorer";
    refresh.handler = []() -> bool { return true; };
    refresh.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(refresh));

    ActionEntry new_file;
    new_file.action_id = "explorer.newFile";
    new_file.label = "New File";
    new_file.handler = []() -> bool { return true; };
    new_file.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(new_file));

    // Stub action (no handler)
    ActionEntry outline;
    outline.action_id = "outline.goTo";
    outline.label = "Go to Symbol";
    outline.validation_status = ActionValidationStatus::kStub;
    manifest.register_action(std::move(outline));

    return manifest;
}

// ============================================================================
// PanelLifecycleState & PanelReadiness — enum tests
// ============================================================================

TEST_CASE("PanelLifecycleState — label conversion", "[v21][p05][panel]")
{
    CHECK(std::string(panel_lifecycle_label(PanelLifecycleState::kRegistered)) == "Registered");
    CHECK(std::string(panel_lifecycle_label(PanelLifecycleState::kActive)) == "Active");
    CHECK(std::string(panel_lifecycle_label(PanelLifecycleState::kStale)) == "Stale");
    CHECK(std::string(panel_lifecycle_label(PanelLifecycleState::kDestroyed)) == "Destroyed");
}

TEST_CASE("PanelReadiness — label conversion", "[v21][p05][panel]")
{
    CHECK(std::string(panel_readiness_label(PanelReadiness::kReady)) == "Ready");
    CHECK(std::string(panel_readiness_label(PanelReadiness::kPlaceholder)) == "Placeholder");
    CHECK(std::string(panel_readiness_label(PanelReadiness::kExperimental)) == "Experimental");
    CHECK(std::string(panel_readiness_label(PanelReadiness::kDeprecated)) == "Deprecated");
}

// ============================================================================
// PanelLifecycleAuditor — Registration
// ============================================================================

TEST_CASE("PanelLifecycleAuditor — register and query panels", "[v21][p05][panel]")
{
    PanelLifecycleAuditor auditor;

    PanelRegistryEntry explorer;
    explorer.panel_id = "explorer";
    explorer.label = "EXPLORER";
    explorer.area = "primary";
    explorer.readiness = PanelReadiness::kReady;
    explorer.has_factory = true;
    explorer.has_real_content = true;
    auditor.register_panel(std::move(explorer));

    PanelRegistryEntry search;
    search.panel_id = "search";
    search.label = "SEARCH";
    search.area = "primary";
    search.readiness = PanelReadiness::kReady;
    search.has_factory = true;
    search.has_real_content = true;
    auditor.register_panel(std::move(search));

    PanelRegistryEntry stub;
    stub.panel_id = "timeline";
    stub.label = "TIMELINE";
    stub.area = "primary";
    stub.readiness = PanelReadiness::kPlaceholder;
    stub.has_factory = true;
    stub.has_real_content = false;
    auditor.register_panel(std::move(stub));

    CHECK(auditor.panel_count() == 3);
    CHECK(auditor.panels_for_area("primary").size() == 3);
    CHECK(auditor.production_panels().size() == 2);
    CHECK(auditor.placeholder_panels().size() == 1);

    auto* found = auditor.get_panel("explorer");
    REQUIRE(found != nullptr);
    CHECK(found->label == "EXPLORER");
    CHECK(found->is_production_visible());
}

// ============================================================================
// PanelLifecycleAuditor — Lifecycle State Transitions
// ============================================================================

TEST_CASE("PanelLifecycleAuditor — lifecycle transitions", "[v21][p05][lifecycle]")
{
    PanelLifecycleAuditor auditor;

    PanelRegistryEntry panel;
    panel.panel_id = "explorer";
    panel.area = "primary";
    panel.readiness = PanelReadiness::kReady;
    panel.has_factory = true;
    panel.has_real_content = true;
    auditor.register_panel(std::move(panel));

    // Default state is kRegistered
    CHECK(auditor.panels_in_state(PanelLifecycleState::kRegistered).size() == 1);

    // Transition to Active
    CHECK(auditor.set_lifecycle("explorer", PanelLifecycleState::kActive));
    CHECK(auditor.panels_in_state(PanelLifecycleState::kActive).size() == 1);
    CHECK(auditor.panels_in_state(PanelLifecycleState::kRegistered).empty());

    // Mark stale
    CHECK(auditor.mark_stale("explorer"));
    CHECK(auditor.panels_in_state(PanelLifecycleState::kStale).size() == 1);

    // Non-existent panel
    CHECK(!auditor.set_lifecycle("nonexistent", PanelLifecycleState::kActive));
}

// ============================================================================
// PanelLifecycleAuditor — Header Actions
// ============================================================================

TEST_CASE("PanelLifecycleAuditor — header actions", "[v21][p05][panel]")
{
    auto manifest = make_panel_manifest();
    PanelLifecycleAuditor auditor;

    PanelHeaderAction collapse;
    collapse.action_id = "panel.collapseAll";
    collapse.panel_id = "explorer";
    collapse.label = "Collapse All";
    auditor.register_header_action(std::move(collapse));

    PanelHeaderAction refresh;
    refresh.action_id = "explorer.refresh";
    refresh.panel_id = "explorer";
    refresh.label = "Refresh";
    auditor.register_header_action(std::move(refresh));

    PanelHeaderAction new_file;
    new_file.action_id = "explorer.newFile";
    new_file.panel_id = "explorer";
    new_file.label = "New File";
    auditor.register_header_action(std::move(new_file));

    auto actions = auditor.header_actions("explorer");
    CHECK(actions.size() == 3);

    // Refresh against manifest
    auditor.refresh_header_actions(manifest);
    for (const auto* action : auditor.header_actions("explorer"))
    {
        CHECK(action->is_bound);
    }
}

// ============================================================================
// PanelLifecycleAuditor — Explorer Sections
// ============================================================================

TEST_CASE("PanelLifecycleAuditor — explorer sections", "[v21][p05][explorer]")
{
    PanelLifecycleAuditor auditor;

    auditor.register_explorer_section({.section_id = "open_editors", .label = "OPEN EDITORS", .is_implemented = true});
    auditor.register_explorer_section({.section_id = "file_tree", .label = "FILES", .is_implemented = true});
    auditor.register_explorer_section({.section_id = "outline", .label = "OUTLINE", .is_implemented = false});
    auditor.register_explorer_section({.section_id = "timeline", .label = "TIMELINE", .is_implemented = false});

    CHECK(auditor.explorer_sections().size() == 4);
    CHECK(auditor.incomplete_explorer_sections().size() == 2);

    // Check incomplete sections are outline and timeline
    bool found_outline = false;
    bool found_timeline = false;
    for (const auto* section : auditor.incomplete_explorer_sections())
    {
        if (section->section_id == "outline") found_outline = true;
        if (section->section_id == "timeline") found_timeline = true;
    }
    CHECK(found_outline);
    CHECK(found_timeline);
}

// ============================================================================
// PanelLifecycleAuditor — Diagnostics
// ============================================================================

TEST_CASE("PanelLifecycleAuditor — diagnostics", "[v21][p05][panel]")
{
    PanelLifecycleAuditor auditor;

    // Ready panel
    PanelRegistryEntry ready;
    ready.panel_id = "explorer";
    ready.area = "primary";
    ready.readiness = PanelReadiness::kReady;
    ready.has_factory = true;
    ready.has_real_content = true;
    auditor.register_panel(std::move(ready));

    // Placeholder panel
    PanelRegistryEntry placeholder;
    placeholder.panel_id = "timeline";
    placeholder.area = "primary";
    placeholder.readiness = PanelReadiness::kPlaceholder;
    placeholder.has_factory = true;
    placeholder.has_real_content = false;
    auditor.register_panel(std::move(placeholder));

    // Stale panel
    PanelRegistryEntry stale;
    stale.panel_id = "search";
    stale.area = "primary";
    stale.readiness = PanelReadiness::kReady;
    stale.has_factory = true;
    stale.has_real_content = true;
    stale.lifecycle = PanelLifecycleState::kStale;
    auditor.register_panel(std::move(stale));

    // Incomplete section
    auditor.register_explorer_section({.section_id = "outline", .label = "OUTLINE", .is_implemented = false});

    auto diags = auditor.diagnose();

    bool found_placeholder = false;
    bool found_dead = false;
    bool found_stale = false;
    bool found_incomplete = false;
    for (const auto& d : diags)
    {
        if (d.is_placeholder) found_placeholder = true;
        if (d.is_dead) found_dead = true;
        if (d.is_stale) found_stale = true;
        if (d.is_incomplete_section) found_incomplete = true;
    }
    CHECK(found_placeholder);
    CHECK(found_dead);   // timeline has no real content
    CHECK(found_stale);
    CHECK(found_incomplete);
}

TEST_CASE("PanelLifecycleAuditor — production vs non-production counts", "[v21][p05][panel]")
{
    PanelLifecycleAuditor auditor;

    PanelRegistryEntry ready;
    ready.panel_id = "explorer";
    ready.readiness = PanelReadiness::kReady;
    ready.has_factory = true;
    ready.has_real_content = true;
    auditor.register_panel(std::move(ready));

    PanelRegistryEntry placeholder;
    placeholder.panel_id = "timeline";
    placeholder.readiness = PanelReadiness::kPlaceholder;
    placeholder.has_factory = true;
    placeholder.has_real_content = false;
    auditor.register_panel(std::move(placeholder));

    CHECK(auditor.production_count() == 1);
    CHECK(auditor.non_production_count() == 1);
}

// ============================================================================
// SidebarStateContract — State Tracking
// ============================================================================

TEST_CASE("SidebarStateContract — panel state tracking", "[v21][p05][state]")
{
    SidebarStateContract contract;

    contract.register_panel("explorer", "primary");
    contract.register_panel("search", "primary");

    // Default state
    CHECK(contract.is_panel_expanded("explorer"));
    CHECK(contract.is_panel_visible("explorer"));

    // Modify state
    contract.set_panel_expanded("explorer", false);
    CHECK(!contract.is_panel_expanded("explorer"));

    contract.set_panel_visible("search", false);
    CHECK(!contract.is_panel_visible("search"));

    // Active panel
    contract.set_active_panel("primary", "explorer");
    CHECK(contract.active_panel("primary") == "explorer");

    // Sidebar width
    contract.set_sidebar_width("primary", 300);
    CHECK(contract.sidebar_width("primary") == 300);
    CHECK(contract.sidebar_width("secondary") == 250); // Default
}

// ============================================================================
// SidebarStateContract — Snapshots
// ============================================================================

TEST_CASE("SidebarStateContract — take and restore snapshot", "[v21][p05][snapshot]")
{
    SidebarStateContract contract;

    contract.register_panel("explorer", "primary");
    contract.register_panel("search", "primary");

    contract.set_active_panel("primary", "explorer");
    contract.set_sidebar_width("primary", 300);
    contract.set_panel_expanded("search", false);

    // Take snapshot
    auto snapshot = contract.take_snapshot("primary");
    CHECK(snapshot.active_panel_id == "explorer");
    CHECK(snapshot.sidebar_width == 300);
    CHECK(snapshot.panel_states.size() == 2);

    // Find panel state in snapshot
    auto* search_state = snapshot.find_panel("search");
    REQUIRE(search_state != nullptr);
    CHECK(!search_state->is_expanded);

    // Modify state
    contract.set_active_panel("primary", "search");
    contract.set_sidebar_width("primary", 200);
    contract.set_panel_expanded("search", true);

    CHECK(contract.active_panel("primary") == "search");
    CHECK(contract.sidebar_width("primary") == 200);

    // Restore from snapshot
    contract.restore_snapshot(snapshot);
    CHECK(contract.active_panel("primary") == "explorer");
    CHECK(contract.sidebar_width("primary") == 300);
    CHECK(!contract.is_panel_expanded("search"));
}

TEST_CASE("SidebarStateContract — named snapshots", "[v21][p05][snapshot]")
{
    SidebarStateContract contract;

    contract.register_panel("explorer", "primary");
    contract.set_active_panel("primary", "explorer");
    contract.set_sidebar_width("primary", 280);

    auto snapshot = contract.take_snapshot("primary");
    contract.save_named_snapshot("zen_pre", snapshot);

    CHECK(contract.has_snapshot("zen_pre"));
    CHECK(!contract.has_snapshot("nonexistent"));

    auto names = contract.snapshot_names();
    REQUIRE(names.size() == 1);
    CHECK(names[0] == "zen_pre");

    auto* retrieved = contract.get_named_snapshot("zen_pre");
    REQUIRE(retrieved != nullptr);
    CHECK(retrieved->active_panel_id == "explorer");
    CHECK(retrieved->sidebar_width == 280);
}

TEST_CASE("SidebarStateContract — panel tracking", "[v21][p05][state]")
{
    SidebarStateContract contract;

    contract.register_panel("explorer", "primary");
    contract.register_panel("search", "primary");
    contract.register_panel("scm", "primary");
    contract.register_panel("output", "bottom");

    CHECK(contract.tracked_panel_count() == 4);
    CHECK(contract.panels_for_area("primary").size() == 3);
    CHECK(contract.panels_for_area("bottom").size() == 1);
    CHECK(contract.panels_for_area("secondary").empty());
}

// ============================================================================
// Integration — lifecycle auditor + state contract lifecycle
// ============================================================================

TEST_CASE("Integration — panel lifecycle + state contract", "[v21][p05][integration]")
{
    auto manifest = make_panel_manifest();

    // 1. Register panels in auditor
    PanelLifecycleAuditor auditor;

    PanelRegistryEntry explorer;
    explorer.panel_id = "explorer";
    explorer.label = "EXPLORER";
    explorer.area = "primary";
    explorer.readiness = PanelReadiness::kReady;
    explorer.has_factory = true;
    explorer.has_real_content = true;
    auditor.register_panel(std::move(explorer));

    PanelRegistryEntry search;
    search.panel_id = "search";
    search.label = "SEARCH";
    search.area = "primary";
    search.readiness = PanelReadiness::kReady;
    search.has_factory = true;
    search.has_real_content = true;
    auditor.register_panel(std::move(search));

    // 2. Register header actions
    PanelHeaderAction collapse;
    collapse.action_id = "panel.collapseAll";
    collapse.panel_id = "explorer";
    auditor.register_header_action(std::move(collapse));

    // 3. Register explorer sections
    auditor.register_explorer_section({.section_id = "open_editors", .label = "OPEN EDITORS", .is_implemented = true});
    auditor.register_explorer_section({.section_id = "outline", .label = "OUTLINE", .is_implemented = false});

    // 4. Setup state contract
    SidebarStateContract state_contract;
    state_contract.register_panel("explorer", "primary");
    state_contract.register_panel("search", "primary");
    state_contract.set_active_panel("primary", "explorer");
    state_contract.set_sidebar_width("primary", 260);

    // 5. Activate explorer
    auditor.set_lifecycle("explorer", PanelLifecycleState::kActive);

    // 6. Take a snapshot before zen mode
    auto pre_zen = state_contract.take_snapshot("primary");
    state_contract.save_named_snapshot("zen_pre", pre_zen);

    // 7. Simulate zen mode (hide everything)
    state_contract.set_panel_visible("explorer", false);
    state_contract.set_panel_visible("search", false);

    // 8. Restore from zen
    auto* saved = state_contract.get_named_snapshot("zen_pre");
    REQUIRE(saved != nullptr);
    state_contract.restore_snapshot(*saved);
    CHECK(state_contract.is_panel_visible("explorer"));
    CHECK(state_contract.active_panel("primary") == "explorer");

    // 9. Refresh header actions
    auditor.refresh_header_actions(manifest);

    // 10. Run diagnostics
    auto diags = auditor.diagnose();
    bool found_incomplete = false;
    for (const auto& d : diags)
    {
        if (d.is_incomplete_section) found_incomplete = true;
    }
    CHECK(found_incomplete); // outline section
    CHECK(auditor.production_count() == 2);
}
