/// @file test_v24_p19_validation_gates.cpp
/// @brief V24 Phase 19 tests: Validation gates, smoke suite, regression packs.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/SubsystemDoneCriteria.h"
#include "../../src/core/ExecutionLedger.h"

using namespace markamp::core;

// P19-T01: Done criterion structure
TEST_CASE("P19-T01 done criterion defaults", "[v24][p19]") {
    DoneCriterion c;
    c.criterion_id = "smoke-test-1";
    c.description = "Core persistence smoke";
    c.is_required = true;
    c.is_passed = false;
    REQUIRE(c.is_blocking());
}

TEST_CASE("P19-T01 done criterion non-blocking", "[v24][p19]") {
    DoneCriterion c;
    c.is_required = false;
    c.is_passed = false;
    REQUIRE_FALSE(c.is_blocking());
}

// P19-T02: Subsystem evaluation
TEST_CASE("P19-T02 subsystem evaluation signoff", "[v24][p19]") {
    SubsystemEvaluation eval;
    eval.total_criteria = 5;
    eval.passed = 5;
    eval.required_passed = 3;
    eval.required_total = 3;
    REQUIRE(eval.is_signed_off());
    REQUIRE(eval.pass_ratio() == 1.0);
}

TEST_CASE("P19-T02 subsystem evaluation blocked", "[v24][p19]") {
    SubsystemEvaluation eval;
    eval.total_criteria = 5;
    eval.passed = 3;
    eval.failed = 2;
    eval.required_passed = 2;
    eval.required_total = 3;
    eval.blocking_criteria = {"smoke-test-3"};
    REQUIRE_FALSE(eval.is_signed_off());
    REQUIRE(eval.has_blockers());
}

// P19-T03: Done criteria management
TEST_CASE("P19-T03 add and evaluate criteria", "[v24][p19]") {
    SubsystemDoneCriteria criteria;
    DoneCriterion c1;
    c1.criterion_id = "test-1"; c1.description = "Test 1"; c1.is_required = true;
    DoneCriterion c2;
    c2.criterion_id = "test-2"; c2.description = "Test 2"; c2.is_required = true;
    criteria.add_criterion(Subsystem::kEditor, c1);
    criteria.add_criterion(Subsystem::kEditor, c2);
    REQUIRE(criteria.total_criteria() == 2);
}

TEST_CASE("P19-T03 mark criterion passed", "[v24][p19]") {
    SubsystemDoneCriteria criteria;
    DoneCriterion c;
    c.criterion_id = "test-1"; c.description = "Test"; c.is_required = true;
    criteria.add_criterion(Subsystem::kEditor, c);
    auto ok = criteria.mark_passed(Subsystem::kEditor, "test-1", "test passed");
    REQUIRE(ok);
    auto eval = criteria.evaluate(Subsystem::kEditor);
    REQUIRE(eval.is_signed_off());
}

// P19-T04: Cross-subsystem evaluation
TEST_CASE("P19-T04 evaluate all subsystems", "[v24][p19]") {
    SubsystemDoneCriteria criteria;
    DoneCriterion c1; c1.criterion_id = "t1"; c1.description = "T1"; c1.is_required = true;
    DoneCriterion c2; c2.criterion_id = "t2"; c2.description = "T2"; c2.is_required = true;
    criteria.add_criterion(Subsystem::kEditor, c1);
    criteria.add_criterion(Subsystem::kEditor, c2);
    criteria.mark_passed(Subsystem::kEditor, "t1", "ok");
    criteria.mark_passed(Subsystem::kEditor, "t2", "ok");
    auto report = criteria.evaluate_all();
    REQUIRE(report.all_signed_off());
}

TEST_CASE("P19-T04 report with blocked subsystem", "[v24][p19]") {
    SubsystemDoneCriteria criteria;
    DoneCriterion c1; c1.criterion_id = "t1"; c1.description = "T1"; c1.is_required = true;
    DoneCriterion c2; c2.criterion_id = "t2"; c2.description = "T2"; c2.is_required = true;
    criteria.add_criterion(Subsystem::kEditor, c1);
    criteria.add_criterion(Subsystem::kEditor, c2);
    criteria.mark_passed(Subsystem::kEditor, "t1", "ok");
    // Canvas t2 not passed
    auto report = criteria.evaluate_all();
    REQUIRE_FALSE(report.all_signed_off());
    REQUIRE(report.blocked >= 1);
}

// P19-T05: Export
TEST_CASE("P19-T05 export JSON", "[v24][p19]") {
    SubsystemDoneCriteria criteria;
    DoneCriterion c; c.criterion_id = "t1"; c.description = "T"; c.is_required = true;
    criteria.add_criterion(Subsystem::kEditor, c);
    auto json = criteria.export_json();
    REQUIRE_FALSE(json.empty());
}

TEST_CASE("P19-T05 export markdown", "[v24][p19]") {
    SubsystemDoneCriteria criteria;
    DoneCriterion c; c.criterion_id = "t1"; c.description = "T"; c.is_required = true;
    criteria.add_criterion(Subsystem::kEditor, c);
    auto md = criteria.export_markdown();
    REQUIRE_FALSE(md.empty());
}
