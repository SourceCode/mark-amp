/// @file test_v21_validation_harness.cpp
/// @brief V21 Phase 10 — Validation harnesses, completeness matrix, panel readiness gate.

#include "core/ControlActionManifest.h"
#include "core/ControlCompletenessMatrix.h"
#include "core/PanelLifecycleAuditor.h"
#include "core/SettingsArchitectureAuditor.h"
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

static auto make_validation_manifest() -> ControlActionManifest {
    ControlActionManifest m;
    for (int i = 0; i < 8; ++i) {
        ActionEntry e;
        e.action_id = "action.live" + std::to_string(i);
        e.label = "Live Action " + std::to_string(i);
        e.handler = []() -> bool { return true; };
        e.validation_status = ActionValidationStatus::kLive;
        m.register_action(std::move(e));
    }
    ActionEntry stub; stub.action_id = "action.stub";
    stub.label = "Stub Action";
    stub.validation_status = ActionValidationStatus::kStub;
    m.register_action(std::move(stub));

    ActionEntry dead; dead.action_id = "action.dead";
    dead.label = "Dead Action";
    dead.validation_status = ActionValidationStatus::kDead;
    m.register_action(std::move(dead));
    return m;
}

TEST_CASE("ControlCompletenessMatrix — manual rows", "[v21][p10][matrix]") {
    ControlCompletenessMatrix matrix;
    matrix.add_row({.control_id = "c1", .label = "Control 1", .surface = "menu",
        .has_handler = true, .validation_status = "Live"});
    matrix.add_row({.control_id = "c2", .label = "Control 2", .surface = "toolbar",
        .has_handler = false, .validation_status = "Dead"});

    CHECK(matrix.row_count() == 2);
    CHECK(matrix.dead_rows().size() == 1);
    CHECK(matrix.rows_for_surface("menu").size() == 1);
}

TEST_CASE("ControlCompletenessMatrix — populate from manifest", "[v21][p10][matrix]") {
    auto manifest = make_validation_manifest();
    ControlCompletenessMatrix matrix;
    matrix.populate_from_manifest(manifest);

    CHECK(matrix.row_count() == 10);
    auto summary = matrix.summarize();
    CHECK(summary.live == 8);
    CHECK(summary.stub == 1);
    CHECK(summary.dead == 1);
    CHECK(summary.total_controls == 10);
    CHECK(summary.pass_rate() == 80.0);
    CHECK(!summary.passes_release_gate()); // Dead > 0
}

TEST_CASE("ControlCompletenessMatrix — release gate logic", "[v21][p10][gate]") {
    MatrixSummary passing;
    passing.total_controls = 100;
    passing.live = 97;
    passing.stub = 3;
    CHECK(passing.passes_release_gate());

    MatrixSummary failing;
    failing.total_controls = 100;
    failing.live = 90;
    failing.dead = 1;
    failing.stub = 9;
    CHECK(!failing.passes_release_gate());
}

TEST_CASE("ControlCompletenessMatrix — markdown output", "[v21][p10][output]") {
    ControlCompletenessMatrix matrix;
    matrix.add_row({.control_id = "test.action", .label = "Test", .has_handler = true, .validation_status = "Live"});
    auto md = matrix.to_markdown();
    CHECK(md.find("Control Completeness Matrix") != std::string::npos);
    CHECK(md.find("test.action") != std::string::npos);
}

TEST_CASE("PanelReadinessGate — report generation", "[v21][p10][panel]") {
    PanelReadinessGate gate;
    gate.add_panel("explorer", PanelReadiness::kReady);
    gate.add_panel("search", PanelReadiness::kReady);
    gate.add_panel("timeline", PanelReadiness::kPlaceholder);
    gate.add_panel("graph", PanelReadiness::kExperimental);

    auto report = gate.generate_report();
    CHECK(report.total_panels == 4);
    CHECK(report.ready == 2);
    CHECK(report.placeholder == 1);
    CHECK(report.experimental == 1);
    CHECK(!report.passes_gate()); // Placeholder > 0
}

TEST_CASE("PanelReadinessGate — clean gate", "[v21][p10][panel]") {
    PanelReadinessGate gate;
    gate.add_panel("explorer", PanelReadiness::kReady);
    gate.add_panel("search", PanelReadiness::kReady);

    auto report = gate.generate_report();
    CHECK(report.passes_gate());
}

TEST_CASE("Integration — full validation harness", "[v21][p10][integration]") {
    auto manifest = make_validation_manifest();

    // 1. Build completeness matrix
    ControlCompletenessMatrix matrix;
    matrix.populate_from_manifest(manifest);
    auto ctrl_summary = matrix.summarize();

    // 2. Build panel readiness gate
    PanelReadinessGate panel_gate;
    panel_gate.add_panel("explorer", PanelReadiness::kReady);
    panel_gate.add_panel("search", PanelReadiness::kReady);
    panel_gate.add_panel("scm", PanelReadiness::kReady);
    auto panel_report = panel_gate.generate_report();

    // 3. Build settings integrity check
    SettingsArchitectureAuditor settings;
    settings.register_setting({.setting_id = "editor.fontSize", .category = "Editor",
        .default_value = "14", .deep_link = "settings://editor.fontSize"});
    settings.set_value("editor.fontSize", SettingScope::kUser, "16");

    // 4. Combined exit criteria
    bool controls_ok = ctrl_summary.live >= 8;
    bool panels_ok = panel_report.passes_gate();
    bool settings_ok = settings.direct_writers().empty();

    CHECK(controls_ok);
    CHECK(panels_ok);
    CHECK(settings_ok);

    // 5. Generate markdown report
    auto md = matrix.to_markdown();
    CHECK(!md.empty());
}
