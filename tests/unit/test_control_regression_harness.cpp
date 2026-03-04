/**
 * @file test_control_regression_harness.cpp
 * @brief Phase 39: Tests for ControlSnapshotTester and KeyboardWorkflowRunner.
 */

#include "ui/ControlSnapshotTester.h"
#include "ui/KeyboardWorkflowRunner.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// ControlSnapshot
// ═══════════════════════════════════════════════════════

TEST_CASE("ControlSnapshot - state name", "[regression][snapshot]")
{
    ControlSnapshot snap;
    snap.state = ControlVisualState::kDefault;
    CHECK(snap.state_name() == "default");

    snap.state = ControlVisualState::kPressed;
    CHECK(snap.state_name() == "pressed");

    snap.state = ControlVisualState::kDisabled;
    CHECK(snap.state_name() == "disabled");
}

// ═══════════════════════════════════════════════════════
// ControlSnapshotTester
// ═══════════════════════════════════════════════════════

TEST_CASE("ControlSnapshotTester - empty state", "[regression][snapshot]")
{
    ControlSnapshotTester tester;
    CHECK(tester.baseline_count() == 0);
}

TEST_CASE("ControlSnapshotTester - register and find", "[regression][snapshot]")
{
    ControlSnapshotTester tester;
    tester.register_baseline({"btn_ok", ControlVisualState::kDefault, "abc123"});

    CHECK(tester.baseline_count() == 1);
    const auto* found = tester.find_baseline("btn_ok", ControlVisualState::kDefault);
    REQUIRE(found != nullptr);
    CHECK(found->expected_hash == "abc123");
}

TEST_CASE("ControlSnapshotTester - compare pass/fail", "[regression][snapshot]")
{
    ControlSnapshotTester tester;
    tester.register_baseline({"btn_ok", ControlVisualState::kDefault, "abc123"});

    ControlSnapshot match;
    match.control_id = "btn_ok";
    match.state = ControlVisualState::kDefault;
    match.hash = "abc123";
    CHECK(tester.compare(match));

    ControlSnapshot mismatch;
    mismatch.control_id = "btn_ok";
    mismatch.state = ControlVisualState::kDefault;
    mismatch.hash = "xyz789";
    CHECK_FALSE(tester.compare(mismatch));
}

TEST_CASE("ControlSnapshotTester - validate all", "[regression][snapshot]")
{
    ControlSnapshotTester tester;
    tester.register_baseline({"a", ControlVisualState::kDefault, "h1"});
    tester.register_baseline({"b", ControlVisualState::kHover, "h2"});

    std::vector<ControlSnapshot> snaps = {
        {"a", ControlVisualState::kDefault, "h1", 0, 0},
        {"b", ControlVisualState::kHover, "h2", 0, 0},
    };
    CHECK(tester.validate_all(snaps) == 2);
}

TEST_CASE("ControlSnapshotTester - failures", "[regression][snapshot]")
{
    ControlSnapshotTester tester;
    tester.register_baseline({"a", ControlVisualState::kDefault, "h1"});

    std::vector<ControlSnapshot> snaps = {
        {"a", ControlVisualState::kDefault, "wrong", 0, 0},
    };
    auto failed = tester.failures(snaps);
    CHECK(failed.size() == 1);
    CHECK(failed[0].control_id == "a");
}

TEST_CASE("ControlSnapshotTester - update baseline", "[regression][snapshot]")
{
    ControlSnapshotTester tester;
    tester.register_baseline({"a", ControlVisualState::kDefault, "old"});

    tester.update_baseline("a", ControlVisualState::kDefault, "new");
    const auto* found = tester.find_baseline("a", ControlVisualState::kDefault);
    REQUIRE(found != nullptr);
    CHECK(found->expected_hash == "new");
}

// ═══════════════════════════════════════════════════════
// WorkflowStep
// ═══════════════════════════════════════════════════════

TEST_CASE("WorkflowStep - result names", "[regression][workflow]")
{
    CHECK(WorkflowStep::result_name(StepResult::kPass) == "pass");
    CHECK(WorkflowStep::result_name(StepResult::kFail) == "fail");
    CHECK(WorkflowStep::result_name(StepResult::kSkipped) == "skipped");
}

// ═══════════════════════════════════════════════════════
// KeyboardWorkflowRunner
// ═══════════════════════════════════════════════════════

TEST_CASE("KeyboardWorkflowRunner - empty state", "[regression][workflow]")
{
    KeyboardWorkflowRunner runner;
    CHECK(runner.workflow_count() == 0);
}

TEST_CASE("KeyboardWorkflowRunner - define workflow", "[regression][workflow]")
{
    KeyboardWorkflowRunner runner;
    runner.begin_workflow("file_save");
    runner.add_step({"s1", "Focus editor", "Tab", []() { return true; }});
    runner.add_step({"s2", "Save file", "Ctrl+S", []() { return true; }});

    CHECK(runner.workflow_count() == 1);
    CHECK(runner.step_count("file_save") == 2);
}

TEST_CASE("KeyboardWorkflowRunner - run passing workflow", "[regression][workflow]")
{
    KeyboardWorkflowRunner runner;
    runner.begin_workflow("test_wf");
    runner.add_step({"s1", "Step 1", "Tab", []() { return true; }});
    runner.add_step({"s2", "Step 2", "Enter", []() { return true; }});

    auto result = runner.run("test_wf");
    CHECK(result.is_pass());
    CHECK(result.total_steps == 2);
    CHECK(result.passed == 2);
    CHECK(result.failed == 0);
}

TEST_CASE("KeyboardWorkflowRunner - run with failures", "[regression][workflow]")
{
    KeyboardWorkflowRunner runner;
    runner.begin_workflow("test_wf");
    runner.add_step({"s1", "Pass", "Tab", []() { return true; }});
    runner.add_step({"s2", "Fail", "Enter", []() { return false; }});

    auto result = runner.run("test_wf");
    CHECK_FALSE(result.is_pass());
    CHECK(result.failed == 1);
    CHECK(result.failure_step_ids.size() == 1);
    CHECK(result.failure_step_ids[0] == "s2");
}

TEST_CASE("KeyboardWorkflowRunner - run all", "[regression][workflow]")
{
    KeyboardWorkflowRunner runner;
    runner.begin_workflow("wf1");
    runner.add_step({"s1", "OK", "Tab", []() { return true; }});

    runner.begin_workflow("wf2");
    runner.add_step({"s1", "Fail", "Esc", []() { return false; }});

    auto results = runner.run_all();
    CHECK(results.size() == 2);
    CHECK(results[0].is_pass());
    CHECK_FALSE(results[1].is_pass());
}

TEST_CASE("KeyboardWorkflowRunner - audit checks", "[regression][workflow]")
{
    KeyboardWorkflowRunner runner;
    runner.add_audit_check("roles", "All controls have roles", []() { return true; });
    runner.add_audit_check("names", "All controls have names", []() { return false; });

    CHECK(runner.audit_check_count() == 2);
    CHECK(runner.run_audit() == 1);
}
