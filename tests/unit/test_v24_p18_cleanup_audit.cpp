/// @file test_v24_p18_cleanup_audit.cpp
/// @brief V24 Phase 18 tests: Migration retirement, cleanup audit, dead code removal.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/SurfaceActionAuditor.h"

using namespace markamp::core;

// P18-T01: Surface control binding structure
TEST_CASE("P18-T01 surface control binding defaults", "[v24][p18]") {
    SurfaceControlBinding binding;
    binding.action_id = "file.save";
    binding.surface_id = "explorer";
    binding.kind = SurfaceControlKind::kToolbarButton;
    REQUIRE(binding.action_id == "file.save");
    REQUIRE(binding.kind == SurfaceControlKind::kToolbarButton);
}

TEST_CASE("P18-T01 surface control kind values", "[v24][p18]") {
    REQUIRE(SurfaceControlKind::kToolbarButton != SurfaceControlKind::kContextMenuItem);
    REQUIRE(SurfaceControlKind::kContextMenuItem != SurfaceControlKind::kInlineAction);
    REQUIRE(SurfaceControlKind::kInlineAction != SurfaceControlKind::kTreeAction);
}

// P18-T02: Auditor registration and query
TEST_CASE("P18-T02 auditor register and count", "[v24][p18]") {
    SurfaceActionAuditor auditor;
    SurfaceControlBinding b;
    b.action_id = "file.save"; b.surface_id = "explorer";
    b.kind = SurfaceControlKind::kToolbarButton;
    auditor.register_control(b);
    REQUIRE(auditor.control_count() == 1);
}

TEST_CASE("P18-T02 auditor controls by surface", "[v24][p18]") {
    SurfaceActionAuditor auditor;
    SurfaceControlBinding b1;
    b1.action_id = "file.save"; b1.surface_id = "explorer"; b1.kind = SurfaceControlKind::kToolbarButton;
    SurfaceControlBinding b2;
    b2.action_id = "file.open"; b2.surface_id = "explorer"; b2.kind = SurfaceControlKind::kToolbarButton;
    SurfaceControlBinding b3;
    b3.action_id = "edit.copy"; b3.surface_id = "search"; b3.kind = SurfaceControlKind::kContextMenuItem;
    auditor.register_control(b1);
    auditor.register_control(b2);
    auditor.register_control(b3);
    auto explorer = auditor.controls_for_surface("explorer");
    REQUIRE(explorer.size() == 2);
    auto surfaces = auditor.all_surfaces();
    REQUIRE(surfaces.size() == 2);
}

// P18-T03: Control gating
TEST_CASE("P18-T03 gate and ungate control", "[v24][p18]") {
    SurfaceActionAuditor auditor;
    SurfaceControlBinding b;
    b.action_id = "debug.start"; b.surface_id = "debug"; b.kind = SurfaceControlKind::kToolbarButton;
    auditor.register_control(b);
    auditor.gate_control("debug.start", "Not implemented");
    REQUIRE(auditor.gated_count() == 1);
    auditor.ungate_control("debug.start");
    REQUIRE(auditor.gated_count() == 0);
}

TEST_CASE("P18-T03 gated controls list", "[v24][p18]") {
    SurfaceActionAuditor auditor;
    SurfaceControlBinding b1;
    b1.action_id = "a"; b1.surface_id = "s"; b1.kind = SurfaceControlKind::kToolbarButton;
    SurfaceControlBinding b2;
    b2.action_id = "b"; b2.surface_id = "s"; b2.kind = SurfaceControlKind::kToolbarButton;
    auditor.register_control(b1);
    auditor.register_control(b2);
    auditor.gate_control("a", "stub");
    auto gated = auditor.gated_controls();
    REQUIRE(gated.size() == 1);
}

// P18-T04: Release gate result
TEST_CASE("P18-T04 release gate result passing", "[v24][p18]") {
    SurfaceActionAuditor::ReleaseGateResult result;
    result.passed = true;
    result.total_controls = 50;
    result.dead_controls = 0;
    result.placeholder_controls = 0;
    REQUIRE(result.passed);
    REQUIRE_FALSE(result.has_blockers());
}

TEST_CASE("P18-T04 release gate result blocking", "[v24][p18]") {
    SurfaceActionAuditor::ReleaseGateResult result;
    result.passed = false;
    result.dead_controls = 3;
    result.blocking_action_ids = {"debug.start", "task.run", "terminal.open"};
    REQUIRE_FALSE(result.passed);
    REQUIRE(result.has_blockers());
}

// P18-T05: Audit entry and control label
TEST_CASE("P18-T05 audit entry dead detection", "[v24][p18]") {
    SurfaceAuditEntry entry;
    entry.action_id = "terminal.open";
    entry.surface_id = "terminal";
    entry.is_dead = true;
    entry.is_placeholder = false;
    REQUIRE(entry.is_dead);
    REQUIRE_FALSE(entry.is_placeholder);
}

TEST_CASE("P18-T05 control kind label", "[v24][p18]") {
    auto label = surface_control_label(SurfaceControlKind::kToolbarButton);
    REQUIRE(std::string(label) == "ToolbarButton");
    auto ctx = surface_control_label(SurfaceControlKind::kContextMenuItem);
    REQUIRE(std::string(ctx) == "ContextMenuItem");
}
