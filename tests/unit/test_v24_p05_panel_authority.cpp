/// @file test_v24_p05_panel_authority.cpp
/// @brief V24 Phase 05 tests: Panel authority, docking, explorer, settings host.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/PanelCapabilityModel.h"
#include "../../src/core/PanelLifecycleAuditor.h"
#include "../../src/core/SidebarStateContract.h"
#include "../../src/core/SettingsArchitectureAuditor.h"
#include "../../src/core/SettingsCatalog.h"

using namespace markamp::core;

// P05-T01: Align panel registries
TEST_CASE("P05-T01 panel capability model registers panels", "[v24][p05]") {
    PanelCapabilityModel model;
    PanelCapabilityEntry entry;
    entry.panel_id = "explorer";
    entry.label = "Explorer";
    entry.host_area = PanelHostArea::kPrimarySidebar;
    entry.capabilities = PanelCapability::kSearchable | PanelCapability::kClosable;
    model.register_panel(entry);

    auto* found = model.get_panel("explorer");
    REQUIRE(found != nullptr);
    REQUIRE(found->has(PanelCapability::kSearchable));
    REQUIRE(found->has(PanelCapability::kClosable));
    REQUIRE(model.panel_count() == 1);
}

TEST_CASE("P05-T01 lifecycle auditor registers panels", "[v24][p05]") {
    PanelLifecycleAuditor auditor;
    PanelRegistryEntry entry;
    entry.panel_id = "explorer";
    entry.label = "Explorer";
    entry.has_factory = true;
    entry.has_real_content = true;
    entry.readiness = PanelReadiness::kReady;
    auditor.register_panel(entry);

    auto* found = auditor.get_panel("explorer");
    REQUIRE(found != nullptr);
    REQUIRE(found->is_production_visible());
    REQUIRE(auditor.panel_count() == 1);
}

// P05-T02: Explorer host authority
TEST_CASE("P05-T02 sidebar state contract visibility", "[v24][p05]") {
    SidebarStateContract contract;
    contract.set_panel_visible("explorer", true);
    contract.set_panel_visible("search", false);
    REQUIRE(contract.is_panel_visible("explorer"));
    REQUIRE_FALSE(contract.is_panel_visible("search"));
}

TEST_CASE("P05-T02 sidebar state active panel", "[v24][p05]") {
    SidebarStateContract contract;
    contract.set_active_panel("primary", "explorer");
    REQUIRE(contract.active_panel("primary") == "explorer");
}

// P05-T03: Settings host authoritative
TEST_CASE("P05-T03 settings catalog lookup", "[v24][p05]") {
    SettingsCatalog catalog;
    CatalogEntry entry;
    entry.setting_id = "editor.fontSize";
    entry.label = "Font Size";
    entry.type = SettingType::Integer;
    entry.default_value = "14";
    catalog.register_setting(entry);

    auto* setting = catalog.find_setting("editor.fontSize");
    REQUIRE(setting != nullptr);
    REQUIRE(setting->label == "Font Size");
}

TEST_CASE("P05-T03 settings architecture auditor", "[v24][p05]") {
    SettingsArchitectureAuditor auditor;
    CatalogSettingEntry entry;
    entry.setting_id = "editor.wordWrap";
    entry.label = "Word Wrap";
    entry.category = "Editor";
    auditor.register_setting(entry);

    REQUIRE(auditor.setting_count() == 1);
    REQUIRE(auditor.get_setting("editor.wordWrap") != nullptr);
}

// P05-T04: Docking and panel lifecycle
TEST_CASE("P05-T04 panel lifecycle state transitions", "[v24][p05]") {
    PanelLifecycleAuditor auditor;
    PanelRegistryEntry entry;
    entry.panel_id = "terminal";
    entry.readiness = PanelReadiness::kPlaceholder;
    entry.has_factory = false;
    auditor.register_panel(entry);

    REQUIRE_FALSE(auditor.get_panel("terminal")->is_production_visible());
    auditor.set_lifecycle("terminal", PanelLifecycleState::kActive);
    auto active_panels = auditor.panels_in_state(PanelLifecycleState::kActive);
    REQUIRE(active_panels.size() == 1);
}

TEST_CASE("P05-T04 stub panels detected", "[v24][p05]") {
    PanelCapabilityModel model;
    PanelCapabilityEntry stub;
    stub.panel_id = "debug";
    stub.label = "Debug";
    stub.is_stub = true;
    model.register_panel(stub);

    auto stubs = model.stub_panels();
    REQUIRE(stubs.size() == 1);
    REQUIRE(stubs[0]->panel_id == "debug");
}

// P05-T05: Panel readiness gates
TEST_CASE("P05-T05 production vs non-production panels", "[v24][p05]") {
    PanelLifecycleAuditor auditor;
    PanelRegistryEntry ready;
    ready.panel_id = "explorer";
    ready.readiness = PanelReadiness::kReady;
    ready.has_factory = true;
    ready.has_real_content = true;
    auditor.register_panel(ready);

    PanelRegistryEntry placeholder;
    placeholder.panel_id = "debug";
    placeholder.readiness = PanelReadiness::kPlaceholder;
    auditor.register_panel(placeholder);

    REQUIRE(auditor.production_count() == 1);
    REQUIRE(auditor.non_production_count() == 1);
}

TEST_CASE("P05-T05 sidebar snapshot round trip", "[v24][p05]") {
    SidebarStateContract contract;
    contract.register_panel("explorer", "primary");
    contract.set_panel_visible("explorer", true);
    contract.set_panel_expanded("explorer", true);
    contract.set_active_panel("primary", "explorer");

    auto snapshot = contract.take_snapshot("primary");
    REQUIRE(snapshot.active_panel_id == "explorer");

    // Restore to a new contract
    SidebarStateContract contract2;
    contract2.restore_snapshot(snapshot);
    REQUIRE(contract2.is_panel_visible("explorer"));
}
