/**
 * @file test_release_audit_gates.cpp
 * @brief Phase 40: Tests for UxAuditRunner and ReleaseGateChecker.
 */

#include "ui/ReleaseGateChecker.h"
#include "ui/UxAuditRunner.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// AuditCheckResult
// ═══════════════════════════════════════════════════════

TEST_CASE("AuditCheckResult - category name", "[audit][check]")
{
    AuditCheckResult result;
    result.category = AuditCategory::kAccessibility;
    CHECK(result.category_name() == "accessibility");

    result.category = AuditCategory::kCrossDpi;
    CHECK(result.category_name() == "cross_dpi");

    result.category = AuditCategory::kPlatform;
    CHECK(result.category_name() == "platform");
}

// ═══════════════════════════════════════════════════════
// UxAuditRunner
// ═══════════════════════════════════════════════════════

TEST_CASE("UxAuditRunner - empty state", "[audit][runner]")
{
    UxAuditRunner runner;
    CHECK(runner.check_count() == 0);
}

TEST_CASE("UxAuditRunner - add and count checks", "[audit][runner]")
{
    UxAuditRunner runner;
    runner.add_check("c1", "Buttons clickable", AuditCategory::kInteraction, []() { return true; });
    runner.add_check("c2", "Contrast OK", AuditCategory::kAccessibility, []() { return true; });

    CHECK(runner.check_count() == 2);
    CHECK(runner.checks_in_category(AuditCategory::kInteraction) == 1);
    CHECK(runner.checks_in_category(AuditCategory::kAccessibility) == 1);
}

TEST_CASE("UxAuditRunner - run all", "[audit][runner]")
{
    UxAuditRunner runner;
    runner.add_check("c1", "Pass", AuditCategory::kInteraction, []() { return true; });
    runner.add_check("c2", "Fail", AuditCategory::kStyle, []() { return false; });

    auto results = runner.run();
    CHECK(results.size() == 2);
    CHECK(results[0].passed);
    CHECK_FALSE(results[1].passed);
}

TEST_CASE("UxAuditRunner - run category", "[audit][runner]")
{
    UxAuditRunner runner;
    runner.add_check("c1", "Interaction", AuditCategory::kInteraction, []() { return true; });
    runner.add_check("c2", "Style", AuditCategory::kStyle, []() { return false; });

    auto interaction_results = runner.run_category(AuditCategory::kInteraction);
    CHECK(interaction_results.size() == 1);
    CHECK(interaction_results[0].passed);
}

TEST_CASE("UxAuditRunner - pass rate", "[audit][runner]")
{
    UxAuditRunner runner;
    runner.add_check("c1", "OK", AuditCategory::kInteraction, []() { return true; });
    runner.add_check("c2", "OK", AuditCategory::kStyle, []() { return true; });
    runner.add_check("c3", "Fail", AuditCategory::kAccessibility, []() { return false; });

    auto results = runner.run();
    double rate = runner.pass_rate(results);
    CHECK(rate > 0.66);
    CHECK(rate < 0.67);
}

TEST_CASE("UxAuditRunner - failure descriptions", "[audit][runner]")
{
    UxAuditRunner runner;
    runner.add_check("c1", "Broken button", AuditCategory::kInteraction, []() { return false; });

    auto results = runner.run();
    auto failures = runner.failure_descriptions(results);
    CHECK(failures.size() == 1);
    CHECK(failures[0].find("Broken button") != std::string::npos);
}

// ═══════════════════════════════════════════════════════
// ReleaseGate
// ═══════════════════════════════════════════════════════

TEST_CASE("ReleaseGate - status name", "[audit][gate]")
{
    ReleaseGate gate;
    gate.status = GateStatus::kPass;
    CHECK(gate.status_name() == "pass");

    gate.status = GateStatus::kBlocked;
    CHECK(gate.status_name() == "blocked");
}

TEST_CASE("ReleaseGate - is blocking", "[audit][gate]")
{
    ReleaseGate gate;
    gate.status = GateStatus::kFail;
    CHECK(gate.is_blocking());

    gate.status = GateStatus::kPass;
    CHECK_FALSE(gate.is_blocking());

    gate.status = GateStatus::kPending;
    CHECK_FALSE(gate.is_blocking());
}

// ═══════════════════════════════════════════════════════
// ReleaseGateChecker
// ═══════════════════════════════════════════════════════

TEST_CASE("ReleaseGateChecker - empty is no-go", "[audit][gate]")
{
    ReleaseGateChecker checker;
    CHECK_FALSE(checker.is_go());
}

TEST_CASE("ReleaseGateChecker - standard gates", "[audit][gate]")
{
    ReleaseGateChecker checker;
    checker.register_standard_gates();

    CHECK(checker.gate_count() == 5);
    CHECK(checker.pending_count() == 5);
    CHECK_FALSE(checker.is_go());
}

TEST_CASE("ReleaseGateChecker - set status and go/no-go", "[audit][gate]")
{
    ReleaseGateChecker checker;
    checker.add_gate({"g1", "Gate 1", "Desc", GateStatus::kPending, ""});
    checker.add_gate({"g2", "Gate 2", "Desc", GateStatus::kPending, ""});

    checker.set_status("g1", GateStatus::kPass);
    checker.set_status("g2", GateStatus::kPass);
    CHECK(checker.is_go());
    CHECK(checker.pass_count() == 2);
}

TEST_CASE("ReleaseGateChecker - blocking gates", "[audit][gate]")
{
    ReleaseGateChecker checker;
    checker.add_gate({"g1", "Gate 1", "Desc", GateStatus::kPass, ""});
    checker.add_gate({"g2", "Gate 2", "Desc", GateStatus::kFail, ""});

    CHECK_FALSE(checker.is_go());
    CHECK(checker.blocking_count() == 1);

    auto blockers = checker.blocking_gates();
    CHECK(blockers.size() == 1);
    CHECK(blockers[0] == "g2");
}
