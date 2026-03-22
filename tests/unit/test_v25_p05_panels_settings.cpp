/// @file test_v25_p05_panels_settings.cpp
/// @brief V25 Phase 05: Panel readiness and settings host tests.
#include <catch2/catch_test_macros.hpp>
#include "core/PanelReadinessController.h"

using namespace markamp::core;

TEST_CASE("V25 P05: Panel classification", "[v25][p05]")
{
    PanelReadinessController ctrl;
    PanelClassification p;
    p.panel_id = "explorer";
    p.scope = PanelScope::kReal;
    p.has_real_factory = true;
    ctrl.classify_panel(p);
    REQUIRE(ctrl.panel_count() == 1);
    REQUIRE(ctrl.get_classification("explorer") != nullptr);
}

TEST_CASE("V25 P05: Gated panels detected", "[v25][p05]")
{
    PanelReadinessController ctrl;
    PanelClassification p;
    p.panel_id = "debug";
    p.scope = PanelScope::kGated;
    ctrl.classify_panel(p);
    REQUIRE(ctrl.gated_panels().size() == 1);
}

TEST_CASE("V25 P05: Blocking panels detected", "[v25][p05]")
{
    PanelReadinessController ctrl;
    PanelClassification p;
    p.panel_id = "output";
    p.scope = PanelScope::kMustFinish;
    p.has_real_factory = false;
    ctrl.classify_panel(p);
    REQUIRE(ctrl.blocking_panels().size() == 1);
}

TEST_CASE("V25 P05: Settings host stages changes", "[v25][p05]")
{
    SettingsHostController host;
    REQUIRE(host.staged_count() == 0);
    host.stage_change("editor.fontSize", "14");
    REQUIRE(host.staged_count() == 1);
    REQUIRE(host.state().has_pending_changes);
}

TEST_CASE("V25 P05: Settings host apply/cancel", "[v25][p05]")
{
    SettingsHostController host;
    host.stage_change("key", "value");
    REQUIRE(host.apply_staged());
    REQUIRE(host.staged_count() == 0);
    REQUIRE_FALSE(host.state().has_pending_changes);
}

TEST_CASE("V25 P05: Settings host deep link", "[v25][p05]")
{
    SettingsHostController host;
    host.activate_deep_link("editor.fontSize");
    REQUIRE(host.state().deep_link_active);
}
