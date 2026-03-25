/// @file test_v24_p20_release_signoff.cpp
/// @brief V24 Phase 20 tests: Release readiness, signoff, gated scope, verdict.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/SubsystemDoneCriteria.h"
#include "../../src/core/ExecutionLedger.h"
#include "../../src/core/DependencyGraphMapper.h"

using namespace markamp::core;

// P20-T01: Done criteria report structure
TEST_CASE("P20-T01 report all signed off", "[v24][p20]") {
    DoneCriteriaReport report;
    report.total_subsystems = 3;
    report.signed_off = 3;
    report.blocked = 0;
    REQUIRE(report.all_signed_off());
    REQUIRE(report.signoff_ratio() == 1.0);
}

TEST_CASE("P20-T01 report partially signed off", "[v24][p20]") {
    DoneCriteriaReport report;
    report.total_subsystems = 5;
    report.signed_off = 3;
    report.blocked = 2;
    REQUIRE_FALSE(report.all_signed_off());
    REQUIRE(report.signoff_ratio() == 0.6);
}

// P20-T02: Subsystem enum coverage
TEST_CASE("P20-T02 subsystem enum distinction", "[v24][p20]") {
    REQUIRE(Subsystem::kGovernance != Subsystem::kEditor);
    REQUIRE(Subsystem::kEditor != Subsystem::kPlugins);
}

TEST_CASE("P20-T02 ledger task status values", "[v24][p20]") {
    REQUIRE(LedgerTaskStatus::kPending != LedgerTaskStatus::kActive);
    REQUIRE(LedgerTaskStatus::kActive != LedgerTaskStatus::kComplete);
    REQUIRE(LedgerTaskStatus::kComplete != LedgerTaskStatus::kGated);
    REQUIRE(LedgerTaskStatus::kGated != LedgerTaskStatus::kDeferred);
}

// P20-T03: Ledger task entry
TEST_CASE("P20-T03 ledger task structure", "[v24][p20]") {
    LedgerTask task;
    task.task_id = "P20-T01";
    task.phase_id = "P20";
    task.title = "Release Signoff";
    task.status = LedgerTaskStatus::kComplete;
    task.subsystem = Subsystem::kGovernance;
    REQUIRE(task.is_complete());
    REQUIRE(task.is_terminal());
}

TEST_CASE("P20-T03 ledger task actionable state", "[v24][p20]") {
    LedgerTask task;
    task.task_id = "P20-T05";
    task.status = LedgerTaskStatus::kPending;
    REQUIRE(task.is_actionable());
    REQUIRE_FALSE(task.is_complete());
}

// P20-T04: Dependency graph
TEST_CASE("P20-T04 dependency graph mapper", "[v24][p20]") {
    DependencyGraphMapper mapper;
    mapper.add_node("P01");
    mapper.add_node("P06");
    [[maybe_unused]] auto ok = mapper.add_dependency("P06", "P01");
    REQUIRE(mapper.node_count() == 2);
    REQUIRE(mapper.edge_count() == 1);
}

TEST_CASE("P20-T04 topological sort", "[v24][p20]") {
    DependencyGraphMapper mapper;
    mapper.add_node("A");
    mapper.add_node("B");
    mapper.add_node("C");
    [[maybe_unused]] auto ok1 = mapper.add_dependency("B", "A");
    [[maybe_unused]] auto ok2 = mapper.add_dependency("C", "B");
    auto sorted = mapper.topological_sort();
    REQUIRE(sorted.size() == 3);
    // Verify all 3 nodes are present in the sort
    REQUIRE(std::find(sorted.begin(), sorted.end(), "A") != sorted.end());
    REQUIRE(std::find(sorted.begin(), sorted.end(), "B") != sorted.end());
    REQUIRE(std::find(sorted.begin(), sorted.end(), "C") != sorted.end());
}

// P20-T05: Full signoff scenario
TEST_CASE("P20-T05 full v24 signoff gate", "[v24][p20]") {
    SubsystemDoneCriteria criteria;
    DoneCriterion c1; c1.criterion_id = "gov-1"; c1.description = "Governance"; c1.is_required = true;
    DoneCriterion c2; c2.criterion_id = "edit-1"; c2.description = "Editor"; c2.is_required = true;
    DoneCriterion c3; c3.criterion_id = "plugin-1"; c3.description = "Canvas"; c3.is_required = true;
    criteria.add_criterion(Subsystem::kGovernance, c1);
    criteria.add_criterion(Subsystem::kEditor, c2);
    criteria.add_criterion(Subsystem::kPlugins, c3);

    [[maybe_unused]] auto r1 = criteria.mark_passed(Subsystem::kGovernance, "gov-1", "All governance tests green");
    [[maybe_unused]] auto r2 = criteria.mark_passed(Subsystem::kEditor, "edit-1", "780/780 tests passing");
    [[maybe_unused]] auto r3 = criteria.mark_passed(Subsystem::kPlugins, "plugin-1", "Plugin lifecycle verified");

    auto report = criteria.evaluate_all();
    REQUIRE(report.all_signed_off());
    REQUIRE(report.blocked == 0);
}

TEST_CASE("P20-T05 ledger report structure", "[v24][p20]") {
    LedgerReport report;
    report.total = 200;
    report.complete = 195;
    report.gated = 3;
    report.deferred = 2;
    report.pending = 0;
    report.active = 0;
    REQUIRE_FALSE(report.has_gaps());
    REQUIRE(report.terminal_count() == 200);
}
