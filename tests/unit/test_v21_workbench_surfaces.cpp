/// @file test_v21_workbench_surfaces.cpp
/// @brief V21 Phase 07 — Tests for Explorer, Search, Problems, Output, Terminal & Debug surfaces.

#include "core/ControlActionManifest.h"
#include "core/SurfaceActionAuditor.h"
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

static auto make_surface_manifest() -> ControlActionManifest {
    ControlActionManifest m;
    for (const auto& id : {"explorer.refresh", "explorer.newFile", "explorer.collapseAll",
                           "search.find", "search.replace", "problems.goTo",
                           "output.clear", "terminal.new", "scm.commit"}) {
        ActionEntry e;
        e.action_id = id;
        e.handler = []() -> bool { return true; };
        e.validation_status = ActionValidationStatus::kLive;
        m.register_action(std::move(e));
    }
    ActionEntry stub; stub.action_id = "debug.start";
    stub.validation_status = ActionValidationStatus::kStub;
    m.register_action(std::move(stub));
    return m;
}

TEST_CASE("SurfaceControlKind — labels", "[v21][p07][surface]") {
    CHECK(std::string(surface_control_label(SurfaceControlKind::kToolbarButton)) == "ToolbarButton");
    CHECK(std::string(surface_control_label(SurfaceControlKind::kQuickFixAction)) == "QuickFixAction");
}

TEST_CASE("SurfaceActionAuditor — registration and surface filtering", "[v21][p07][surface]") {
    SurfaceActionAuditor auditor;
    auditor.register_controls({
        {.action_id = "explorer.refresh", .surface_id = "explorer", .label = "Refresh", .kind = SurfaceControlKind::kToolbarButton, .is_bound = true},
        {.action_id = "explorer.newFile", .surface_id = "explorer", .label = "New File", .kind = SurfaceControlKind::kToolbarButton, .is_bound = true},
        {.action_id = "search.find", .surface_id = "search", .label = "Find", .kind = SurfaceControlKind::kSearchAction, .is_bound = true},
        {.action_id = "problems.goTo", .surface_id = "problems", .label = "Go To", .kind = SurfaceControlKind::kNavigationAction, .is_bound = true},
    });

    CHECK(auditor.control_count() == 4);
    CHECK(auditor.controls_for_surface("explorer").size() == 2);
    CHECK(auditor.controls_for_surface("search").size() == 1);
    CHECK(auditor.controls_by_kind(SurfaceControlKind::kToolbarButton).size() == 2);

    auto surfaces = auditor.all_surfaces();
    CHECK(surfaces.size() == 3);
}

TEST_CASE("SurfaceActionAuditor — gating controls", "[v21][p07][gating]") {
    SurfaceActionAuditor auditor;
    auditor.register_control({.action_id = "debug.start", .surface_id = "debug", .is_bound = true});

    auditor.gate_control("debug.start", "Debug not yet fully implemented");
    CHECK(auditor.gated_count() == 1);
    auto* c = auditor.get_control("debug.start");
    REQUIRE(c != nullptr);
    CHECK(c->is_gated);
    CHECK(c->gate_reason == "Debug not yet fully implemented");

    auditor.ungate_control("debug.start");
    CHECK(auditor.gated_count() == 0);
}

TEST_CASE("SurfaceActionAuditor — manifest refresh and audit", "[v21][p07][surface]") {
    auto manifest = make_surface_manifest();
    SurfaceActionAuditor auditor;
    auditor.register_controls({
        {.action_id = "explorer.refresh", .surface_id = "explorer"},
        {.action_id = "debug.start", .surface_id = "debug"},
        {.action_id = "orphan.action", .surface_id = "unknown"},
    });

    auditor.refresh_from_manifest(manifest);
    CHECK(auditor.get_control("explorer.refresh")->is_bound);
    CHECK(!auditor.get_control("debug.start")->is_bound);

    auto issues = auditor.audit(manifest);
    bool found_dead = false, found_placeholder = false;
    for (const auto& e : issues) {
        if (e.action_id == "orphan.action" && e.is_dead) found_dead = true;
        if (e.action_id == "debug.start" && e.is_placeholder) found_placeholder = true;
    }
    CHECK(found_dead);
    CHECK(found_placeholder);
}

TEST_CASE("SurfaceActionAuditor — live/dead/gated counts", "[v21][p07][surface]") {
    SurfaceActionAuditor auditor;
    auditor.register_controls({
        {.action_id = "live", .surface_id = "s", .is_bound = true, .is_enabled = true},
        {.action_id = "dead", .surface_id = "s", .is_bound = false},
        {.action_id = "gated", .surface_id = "s", .is_bound = true, .is_enabled = true, .is_gated = true},
    });
    CHECK(auditor.live_count() == 1);
    CHECK(auditor.dead_count() == 1);
    CHECK(auditor.gated_count() == 1);
}

TEST_CASE("Integration — all workbench surfaces audit", "[v21][p07][integration]") {
    auto manifest = make_surface_manifest();
    SurfaceActionAuditor auditor;

    // Register controls across all surfaces
    auditor.register_controls({
        {.action_id = "explorer.refresh", .surface_id = "explorer", .kind = SurfaceControlKind::kToolbarButton},
        {.action_id = "explorer.newFile", .surface_id = "explorer", .kind = SurfaceControlKind::kToolbarButton},
        {.action_id = "explorer.collapseAll", .surface_id = "explorer", .kind = SurfaceControlKind::kHeaderAction},
        {.action_id = "search.find", .surface_id = "search", .kind = SurfaceControlKind::kSearchAction},
        {.action_id = "search.replace", .surface_id = "search", .kind = SurfaceControlKind::kSearchAction},
        {.action_id = "problems.goTo", .surface_id = "problems", .kind = SurfaceControlKind::kNavigationAction},
        {.action_id = "output.clear", .surface_id = "output", .kind = SurfaceControlKind::kToolbarButton},
        {.action_id = "terminal.new", .surface_id = "terminal", .kind = SurfaceControlKind::kToolbarButton},
        {.action_id = "scm.commit", .surface_id = "scm", .kind = SurfaceControlKind::kToolbarButton},
        {.action_id = "debug.start", .surface_id = "debug", .kind = SurfaceControlKind::kToolbarButton},
    });

    auditor.refresh_from_manifest(manifest);
    CHECK(auditor.all_surfaces().size() == 7);
    CHECK(auditor.live_count() == 9);  // All except debug.start
    CHECK(auditor.dead_count() == 1);  // debug.start stub

    auto issues = auditor.audit(manifest);
    CHECK(!issues.empty());
}
