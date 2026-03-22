/// @file test_v24_p01_execution_ledger.cpp
/// @brief V24 Phase 01 tests: ExecutionLedger, SubsystemDoneCriteria, DependencyGraphMapper,
///        SurfaceActionAuditor release gates, and WorkflowSmokeRunner phase readiness.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/ExecutionLedger.h"
#include "../../src/core/SubsystemDoneCriteria.h"
#include "../../src/core/DependencyGraphMapper.h"
#include "../../src/core/WorkflowSmokeRunner.h"

using namespace markamp::core;

// ════════════════════════════════════════════════════════════════
// ExecutionLedger Tests (P01-T01)
// ════════════════════════════════════════════════════════════════

TEST_CASE("P01-T01 ledger registration and lookup", "[v24][p01]") {
    ExecutionLedger ledger;
    LedgerTask task;
    task.task_id = "P02-T01";
    task.phase_id = "P02";
    task.title = "Replace pseudo-untitled paths";
    task.subsystem = Subsystem::kArtifactLifecycle;
    REQUIRE(ledger.register_task(task));
    REQUIRE(ledger.task_count() == 1);
    REQUIRE_FALSE(ledger.register_task(task));   // duplicate

    auto* found = ledger.get_task("P02-T01");
    REQUIRE(found != nullptr);
    REQUIRE(found->title == "Replace pseudo-untitled paths");
    REQUIRE(found->is_actionable());
}

TEST_CASE("P01-T01 ledger status transitions", "[v24][p01]") {
    ExecutionLedger ledger;
    LedgerTask task;
    task.task_id = "P03-T01";
    task.phase_id = "P03";
    task.subsystem = Subsystem::kPersistence;
    REQUIRE(ledger.register_task(task));

    REQUIRE(ledger.activate_task("P03-T01"));
    REQUIRE(ledger.get_task("P03-T01")->status == LedgerTaskStatus::kActive);

    REQUIRE(ledger.complete_task("P03-T01"));
    REQUIRE(ledger.get_task("P03-T01")->is_complete());

    REQUIRE(ledger.gate_task("P03-T01"));
    REQUIRE(ledger.get_task("P03-T01")->status == LedgerTaskStatus::kGated);

    REQUIRE_FALSE(ledger.complete_task("nonexistent"));
}

TEST_CASE("P01-T01 ledger queries", "[v24][p01]") {
    ExecutionLedger ledger;
    LedgerTask t1{.task_id = "P02-T01", .phase_id = "P02", .subsystem = Subsystem::kArtifactLifecycle};
    LedgerTask t2{.task_id = "P02-T02", .phase_id = "P02", .subsystem = Subsystem::kArtifactLifecycle};
    LedgerTask t3{.task_id = "P03-T01", .phase_id = "P03", .subsystem = Subsystem::kPersistence};
    ledger.register_task(t1);
    ledger.register_task(t2);
    ledger.register_task(t3);

    auto phase2 = ledger.query_by_phase("P02");
    REQUIRE(phase2.size() == 2);

    auto artifact_tasks = ledger.query_by_subsystem(Subsystem::kArtifactLifecycle);
    REQUIRE(artifact_tasks.size() == 2);

    ledger.complete_task("P02-T01");
    auto complete = ledger.query_by_status(LedgerTaskStatus::kComplete);
    REQUIRE(complete.size() == 1);
}

TEST_CASE("P01-T01 ledger dependencies", "[v24][p01]") {
    ExecutionLedger ledger;
    LedgerTask t1{.task_id = "P01-T01", .phase_id = "P01"};
    LedgerTask t2{.task_id = "P01-T02", .phase_id = "P01", .dependencies = {"P01-T01"}};
    LedgerTask t3{.task_id = "P01-T03", .phase_id = "P01", .dependencies = {"P01-T02"}};
    ledger.register_task(t1);
    ledger.register_task(t2);
    ledger.register_task(t3);

    REQUIRE_FALSE(ledger.dependencies_met("P01-T02"));
    ledger.complete_task("P01-T01");
    REQUIRE(ledger.dependencies_met("P01-T02"));

    auto chain = ledger.dependency_chain("P01-T03");
    REQUIRE(chain.size() == 2); // P01-T02 and P01-T01
}

TEST_CASE("P01-T01 ledger report and export", "[v24][p01]") {
    ExecutionLedger ledger;
    LedgerTask t1{.task_id = "T1", .phase_id = "P01"};
    LedgerTask t2{.task_id = "T2", .phase_id = "P01"};
    ledger.register_task(t1);
    ledger.register_task(t2);
    ledger.complete_task("T1");

    auto r = ledger.report();
    REQUIRE(r.total == 2);
    REQUIRE(r.complete == 1);
    REQUIRE(r.pending == 1);
    REQUIRE(r.has_gaps());

    auto json = ledger.export_json();
    REQUIRE(json.find("\"total\": 2") != std::string::npos);

    auto md = ledger.export_markdown();
    REQUIRE(md.find("Execution Ledger") != std::string::npos);

    ledger.clear();
    REQUIRE(ledger.task_count() == 0);
}

// ════════════════════════════════════════════════════════════════
// SubsystemDoneCriteria Tests (P01-T02)
// ════════════════════════════════════════════════════════════════

TEST_CASE("P01-T02 done criteria registration and evaluation", "[v24][p01]") {
    SubsystemDoneCriteria criteria;
    criteria.add_criterion(Subsystem::kSearch, {
        .criterion_id = "search-smoke", .description = "Search smoke test", .is_required = true
    });
    criteria.add_criterion(Subsystem::kSearch, {
        .criterion_id = "search-panel", .description = "Search panel test", .is_required = true
    });

    auto eval = criteria.evaluate(Subsystem::kSearch);
    REQUIRE(eval.total_criteria == 2);
    REQUIRE_FALSE(eval.is_signed_off());
    REQUIRE(eval.has_blockers());

    REQUIRE(criteria.mark_passed(Subsystem::kSearch, "search-smoke", "test_search.cpp"));
    REQUIRE(criteria.mark_passed(Subsystem::kSearch, "search-panel", "test_panel.cpp"));

    eval = criteria.evaluate(Subsystem::kSearch);
    REQUIRE(eval.is_signed_off());
    REQUIRE_FALSE(eval.has_blockers());
}

TEST_CASE("P01-T02 done criteria report", "[v24][p01]") {
    SubsystemDoneCriteria criteria;
    criteria.add_criterion(Subsystem::kEditor, {.criterion_id = "editor-smoke", .is_required = true, .is_passed = true});
    criteria.add_criterion(Subsystem::kSearch, {.criterion_id = "search-smoke", .is_required = true});

    auto report = criteria.evaluate_all();
    REQUIRE(report.total_subsystems == 2);
    REQUIRE(report.signed_off == 1);
    REQUIRE(report.blocked == 1);
    REQUIRE_FALSE(report.all_signed_off());

    auto json = criteria.export_json();
    REQUIRE(json.find("\"signed_off\": 1") != std::string::npos);

    auto md = criteria.export_markdown();
    REQUIRE(md.find("Subsystem Done Criteria") != std::string::npos);

    criteria.clear();
    REQUIRE(criteria.total_criteria() == 0);
}

// ════════════════════════════════════════════════════════════════
// DependencyGraphMapper Tests (P01-T03)
// ════════════════════════════════════════════════════════════════

TEST_CASE("P01-T03 graph construction and cycle detection", "[v24][p01]") {
    DependencyGraphMapper graph;
    graph.add_node("A");
    graph.add_node("B");
    graph.add_node("C");
    REQUIRE(graph.node_count() == 3);

    REQUIRE(graph.add_dependency("A", "B"));
    REQUIRE(graph.add_dependency("B", "C"));
    REQUIRE(graph.edge_count() == 2);

    // Cycle detection: C -> A would form A -> B -> C -> A
    REQUIRE(graph.would_create_cycle("C", "A"));
    REQUIRE_FALSE(graph.add_dependency("C", "A"));
}

TEST_CASE("P01-T03 topological sort and critical path", "[v24][p01]") {
    DependencyGraphMapper graph;
    // A -> B -> D, A -> C -> D
    graph.add_dependency("A", "B");
    graph.add_dependency("A", "C");
    graph.add_dependency("B", "D");
    graph.add_dependency("C", "D");

    auto sorted = graph.topological_sort();
    REQUIRE(sorted.size() == 4);
    // A must come before B, C, D
    auto pos_a = std::find(sorted.begin(), sorted.end(), "A") - sorted.begin();
    auto pos_d = std::find(sorted.begin(), sorted.end(), "D") - sorted.begin();
    REQUIRE(pos_a < pos_d);

    auto cp = graph.critical_path();
    REQUIRE(cp.size() >= 3); // At least A -> B -> D or A -> C -> D
}

TEST_CASE("P01-T03 parallel groups and analysis", "[v24][p01]") {
    DependencyGraphMapper graph;
    graph.add_dependency("A", "B");
    graph.add_dependency("A", "C");
    graph.add_dependency("B", "D");
    graph.add_dependency("C", "D");

    auto groups = graph.parallel_groups();
    REQUIRE(groups.size() >= 2); // At least: {A}, {B,C}, {D}

    auto analysis = graph.analyze();
    REQUIRE(analysis.is_valid());
    REQUIRE_FALSE(analysis.has_cycles);
    REQUIRE(analysis.total_nodes == 4);
    REQUIRE(analysis.total_edges == 4);

    auto mermaid = graph.export_mermaid();
    REQUIRE(mermaid.find("graph TD") != std::string::npos);

    graph.clear();
    REQUIRE(graph.node_count() == 0);
}

// ════════════════════════════════════════════════════════════════
// WorkflowSmokeRunner Phase Readiness Tests (P01-T05)
// ════════════════════════════════════════════════════════════════

TEST_CASE("P01-T05 phase readiness check", "[v24][p01]") {
    EventBus bus;
    WorkflowSmokeRunner runner(bus);

    runner.register_test("P02.smoke.create", "Artifact creation", []() {
        SmokeTestResult r;
        r.status = SmokeTestStatus::kPassed;
        return r;
    });
    runner.register_test("P02.smoke.save", "Artifact save", []() {
        SmokeTestResult r;
        r.status = SmokeTestStatus::kPassed;
        return r;
    });
    runner.register_test("P03.smoke.autosave", "Autosave", []() {
        SmokeTestResult r;
        r.status = SmokeTestStatus::kFailed;
        r.error_message = "Not yet implemented";
        return r;
    });

    auto p02_readiness = runner.check_phase_readiness("P02");
    REQUIRE(p02_readiness.is_ready);
    REQUIRE(p02_readiness.total_tests == 2);
    REQUIRE(p02_readiness.passed_tests == 2);
    REQUIRE_FALSE(p02_readiness.has_blockers());

    auto p03_readiness = runner.check_phase_readiness("P03");
    REQUIRE_FALSE(p03_readiness.is_ready);
    REQUIRE(p03_readiness.failed_tests == 1);
    REQUIRE(p03_readiness.has_blockers());
}
