/// @file test_v21_panel_lifecycle.cpp
/// @brief V21 Phase 06 — Tests for Secondary, Bottom & Specialized Panel Lifecycle.

#include "core/ControlActionManifest.h"
#include "core/PanelCapabilityModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

static auto make_panel_lifecycle_manifest() -> ControlActionManifest
{
    ControlActionManifest m;
    ActionEntry toggle_output;
    toggle_output.action_id = "panel.toggleOutput";
    toggle_output.label = "Toggle Output";
    toggle_output.handler = []() -> bool { return true; };
    toggle_output.validation_status = ActionValidationStatus::kLive;
    m.register_action(std::move(toggle_output));

    ActionEntry toggle_terminal;
    toggle_terminal.action_id = "panel.toggleTerminal";
    toggle_terminal.label = "Toggle Terminal";
    toggle_terminal.handler = []() -> bool { return true; };
    toggle_terminal.validation_status = ActionValidationStatus::kLive;
    m.register_action(std::move(toggle_terminal));

    ActionEntry toggle_outline;
    toggle_outline.action_id = "panel.toggleOutline";
    toggle_outline.label = "Toggle Outline";
    toggle_outline.validation_status = ActionValidationStatus::kStub;
    m.register_action(std::move(toggle_outline));
    return m;
}

TEST_CASE("PanelCapability — bitwise operations", "[v21][p06][capability]")
{
    auto caps = PanelCapability::kSearchable | PanelCapability::kClosable | PanelCapability::kResizable;
    CHECK(has_capability(caps, PanelCapability::kSearchable));
    CHECK(has_capability(caps, PanelCapability::kClosable));
    CHECK(!has_capability(caps, PanelCapability::kDraggable));
}

TEST_CASE("PanelHostArea — label conversion", "[v21][p06][capability]")
{
    CHECK(std::string(panel_host_label(PanelHostArea::kBottomPanel)) == "BottomPanel");
    CHECK(std::string(panel_host_label(PanelHostArea::kSecondarySidebar)) == "SecondarySidebar");
}

TEST_CASE("PanelCapabilityModel — registration and area filtering", "[v21][p06][panel]")
{
    PanelCapabilityModel model;

    model.register_panel({.panel_id = "output", .label = "OUTPUT",
                          .host_area = PanelHostArea::kBottomPanel,
                          .capabilities = PanelCapability::kSearchable | PanelCapability::kClosable});
    model.register_panel({.panel_id = "terminal", .label = "TERMINAL",
                          .host_area = PanelHostArea::kBottomPanel,
                          .capabilities = PanelCapability::kResizable | PanelCapability::kClosable});
    model.register_panel({.panel_id = "outline", .label = "OUTLINE",
                          .host_area = PanelHostArea::kSecondarySidebar, .is_stub = true});

    CHECK(model.panel_count() == 3);
    CHECK(model.panels_for_area(PanelHostArea::kBottomPanel).size() == 2);
    CHECK(model.panels_for_area(PanelHostArea::kSecondarySidebar).size() == 1);
    CHECK(model.stub_panels().size() == 1);
    CHECK(model.live_panel_count() == 2);
    CHECK(model.stub_count() == 1);

    auto* p = model.get_panel("output");
    REQUIRE(p != nullptr);
    CHECK(p->has(PanelCapability::kSearchable));
    CHECK(!p->has(PanelCapability::kDraggable));
}

TEST_CASE("PanelCapabilityModel — toggle commands", "[v21][p06][toggle]")
{
    auto manifest = make_panel_lifecycle_manifest();
    PanelCapabilityModel model;

    model.register_panel({.panel_id = "output", .host_area = PanelHostArea::kBottomPanel});
    model.register_toggle({.panel_id = "output", .action_id = "panel.toggleOutput"});

    CHECK(model.dispatch_toggle("output", manifest));
    CHECK(!model.dispatch_toggle("nonexistent", manifest));

    model.refresh_toggles(manifest);
    auto* toggle = model.get_toggle("output");
    REQUIRE(toggle != nullptr);
    CHECK(toggle->is_bound);
}

TEST_CASE("PanelCapabilityModel — layout snapshots", "[v21][p06][snapshot]")
{
    PanelCapabilityModel model;

    model.register_panel({.panel_id = "output", .host_area = PanelHostArea::kBottomPanel});
    model.register_panel({.panel_id = "terminal", .host_area = PanelHostArea::kBottomPanel});

    model.set_active_bottom("output");
    model.set_bottom_height(300);
    model.set_bottom_visible(true);
    model.set_layout_state("output", true, -1, 300);
    model.set_layout_state("terminal", false);

    auto snapshot = model.take_layout_snapshot();
    CHECK(snapshot.active_bottom_panel == "output");
    CHECK(snapshot.bottom_panel_height == 300);
    CHECK(snapshot.bottom_visible);
    CHECK(snapshot.panel_states.size() == 2);

    // Modify state
    model.set_active_bottom("terminal");
    model.set_bottom_height(150);
    model.set_bottom_visible(false);

    // Restore
    model.restore_layout_snapshot(snapshot);
    auto restored = model.take_layout_snapshot();
    CHECK(restored.active_bottom_panel == "output");
    CHECK(restored.bottom_panel_height == 300);
    CHECK(restored.bottom_visible);
}

TEST_CASE("PanelCapabilityModel — diagnostics", "[v21][p06][panel]")
{
    auto manifest = make_panel_lifecycle_manifest();
    PanelCapabilityModel model;

    model.register_panel({.panel_id = "output", .host_area = PanelHostArea::kBottomPanel});
    model.register_panel({.panel_id = "history", .host_area = PanelHostArea::kBottomPanel, .is_stub = true});
    model.register_panel({.panel_id = "outline", .host_area = PanelHostArea::kSecondarySidebar});

    model.register_toggle({.panel_id = "output", .action_id = "panel.toggleOutput"});
    model.register_toggle({.panel_id = "outline", .action_id = "panel.toggleOutline"});
    // history has no toggle

    auto diags = model.diagnose(manifest);
    bool found_stub = false, found_missing_toggle = false, found_missing_handler = false;
    for (const auto& d : diags)
    {
        if (d.is_stub) found_stub = true;
        if (d.is_missing_toggle) found_missing_toggle = true;
        if (d.is_missing_handler) found_missing_handler = true;
    }
    CHECK(found_stub);
    CHECK(found_missing_toggle);
    CHECK(found_missing_handler); // outline toggle is stub
}

TEST_CASE("Integration — panel lifecycle + capabilities + toggles", "[v21][p06][integration]")
{
    auto manifest = make_panel_lifecycle_manifest();
    PanelCapabilityModel model;

    model.register_panel({.panel_id = "output", .label = "OUTPUT",
                          .host_area = PanelHostArea::kBottomPanel,
                          .capabilities = PanelCapability::kSearchable | PanelCapability::kClosable | PanelCapability::kPersistable});
    model.register_panel({.panel_id = "terminal", .label = "TERMINAL",
                          .host_area = PanelHostArea::kBottomPanel,
                          .capabilities = PanelCapability::kResizable | PanelCapability::kClosable | PanelCapability::kPersistable});

    model.register_toggle({.panel_id = "output", .action_id = "panel.toggleOutput"});
    model.register_toggle({.panel_id = "terminal", .action_id = "panel.toggleTerminal"});

    // Setup layout
    model.set_active_bottom("output");
    model.set_bottom_height(250);
    model.set_bottom_visible(true);

    // Take snapshot
    auto snap = model.take_layout_snapshot();

    // Toggle terminal
    CHECK(model.dispatch_toggle("terminal", manifest));
    model.set_active_bottom("terminal");

    // Restore to output
    model.restore_layout_snapshot(snap);
    auto restored = model.take_layout_snapshot();
    CHECK(restored.active_bottom_panel == "output");

    // Refresh toggles
    model.refresh_toggles(manifest);
    CHECK(model.get_toggle("output")->is_bound);

    CHECK(model.live_panel_count() == 2);
}
