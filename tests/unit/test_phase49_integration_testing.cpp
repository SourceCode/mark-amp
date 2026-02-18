/// @file test_phase49_integration_testing.cpp
/// @brief Phase 49 tests — End-to-End Integration Testing.

#include "core/CrossModuleTestSuite.h"
#include "core/Events.h"
#include "core/IntegrationTestCommandProvider.h"
#include "core/IntegrationTestRunner.h"
#include "core/RegressionTestEngine.h"
#include "core/SystemHealthValidator.h"
#include "core/ValidationCommandProvider.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// IntegrationTestRunner
// ============================================================================

TEST_CASE("IntegrationTestRunner initial state", "[phase49][runner]")
{
    IntegrationTestRunner runner;
    REQUIRE(runner.suite_count() == 0);
    auto sum = runner.summary();
    REQUIRE(sum.total_tests == 0);
}

TEST_CASE("IntegrationTestRunner create suite", "[phase49][runner]")
{
    IntegrationTestRunner runner;
    auto sid = runner.create_suite("Core Tests");
    REQUIRE_FALSE(sid.empty());
    REQUIRE(runner.suite_count() == 1);
    const auto* suite = runner.get_suite(sid);
    REQUIRE(suite != nullptr);
    REQUIRE(suite->name == "Core Tests");
}

TEST_CASE("IntegrationTestRunner add results", "[phase49][runner]")
{
    IntegrationTestRunner runner;
    auto sid = runner.create_suite("Suite");
    TestResult res;
    res.test_id = "t1";
    res.name = "Test 1";
    res.status = TestStatus::kPassed;
    res.duration_ms = 5.0;
    REQUIRE(runner.add_result(sid, res));

    const auto* suite = runner.get_suite(sid);
    REQUIRE(suite->results.size() == 1);
    REQUIRE(suite->passed_count() == 1);
}

TEST_CASE("IntegrationTestRunner run all marks pending as passed", "[phase49][runner]")
{
    IntegrationTestRunner runner;
    auto sid = runner.create_suite("Auto");
    TestResult res;
    res.test_id = "t1";
    res.name = "Auto Test";
    res.status = TestStatus::kPending;
    runner.add_result(sid, res);

    runner.run_all();
    const auto* suite = runner.get_suite(sid);
    REQUIRE(suite->results[0].status == TestStatus::kPassed);
}

TEST_CASE("IntegrationTestRunner summary aggregation", "[phase49][runner]")
{
    IntegrationTestRunner runner;
    auto sid1 = runner.create_suite("Suite A");
    auto sid2 = runner.create_suite("Suite B");

    TestResult pass_res;
    pass_res.test_id = "t1";
    pass_res.status = TestStatus::kPassed;
    pass_res.duration_ms = 2.0;
    runner.add_result(sid1, pass_res);

    TestResult fail_res;
    fail_res.test_id = "t2";
    fail_res.status = TestStatus::kFailed;
    fail_res.error_msg = "assertion failed";
    runner.add_result(sid2, fail_res);

    auto sum = runner.summary();
    REQUIRE(sum.total_suites == 2);
    REQUIRE(sum.total_tests == 2);
    REQUIRE(sum.passed == 1);
    REQUIRE(sum.failed == 1);
    REQUIRE_FALSE(sum.all_passed());
}

TEST_CASE("IntegrationTestRunner clear", "[phase49][runner]")
{
    IntegrationTestRunner runner;
    runner.create_suite("Temp");
    runner.clear();
    REQUIRE(runner.suite_count() == 0);
}

// ============================================================================
// CrossModuleTestSuite
// ============================================================================

TEST_CASE("CrossModuleTestSuite initial state", "[phase49][crossmod]")
{
    CrossModuleTestSuite suite;
    REQUIRE(suite.module_count() == 0);
    REQUIRE(suite.dependency_count() == 0);
}

TEST_CASE("CrossModuleTestSuite register modules", "[phase49][crossmod]")
{
    CrossModuleTestSuite suite;
    suite.register_module("core");
    suite.register_module("ui");
    REQUIRE(suite.module_count() == 2);
    REQUIRE(suite.has_module("core"));
    REQUIRE_FALSE(suite.has_module("network"));
}

TEST_CASE("CrossModuleTestSuite add and check dependencies", "[phase49][crossmod]")
{
    CrossModuleTestSuite suite;
    suite.register_module("core");
    suite.register_module("ui");
    suite.add_dependency("ui", "core");

    auto deps = suite.check_dependencies();
    REQUIRE(deps.size() == 1);
    REQUIRE(deps[0].is_satisfied);
}

TEST_CASE("CrossModuleTestSuite unsatisfied dependency", "[phase49][crossmod]")
{
    CrossModuleTestSuite suite;
    suite.register_module("ui");
    suite.add_dependency("ui", "missing_mod");

    auto deps = suite.check_dependencies();
    REQUIRE(deps.size() == 1);
    REQUIRE_FALSE(deps[0].is_satisfied);
}

TEST_CASE("CrossModuleTestSuite compatibility check", "[phase49][crossmod]")
{
    CrossModuleTestSuite suite;
    suite.register_module("core");
    suite.register_module("ui");
    suite.add_dependency("ui", "core");

    auto compat = suite.run_compatibility_check();
    REQUIRE(compat.size() == 2);
    for (const auto& res : compat)
    {
        REQUIRE(res.compatible);
    }
}

TEST_CASE("CrossModuleTestSuite clear", "[phase49][crossmod]")
{
    CrossModuleTestSuite suite;
    suite.register_module("core");
    suite.add_dependency("core", "ext");
    suite.clear();
    REQUIRE(suite.module_count() == 0);
    REQUIRE(suite.dependency_count() == 0);
}

// ============================================================================
// SystemHealthValidator
// ============================================================================

TEST_CASE("SystemHealthValidator initial state", "[phase49][health]")
{
    SystemHealthValidator validator;
    REQUIRE(validator.check_count() == 0);
    REQUIRE_FALSE(validator.is_healthy()); // empty = not healthy
}

TEST_CASE("SystemHealthValidator load defaults", "[phase49][health]")
{
    SystemHealthValidator validator;
    validator.load_default_checks();
    REQUIRE(validator.check_count() == 10);
    REQUIRE(validator.is_healthy());
    REQUIRE(validator.passed_count() == 10);
    REQUIRE(validator.failed_count() == 0);
}

TEST_CASE("SystemHealthValidator add failing check", "[phase49][health]")
{
    SystemHealthValidator validator;
    validator.load_default_checks();
    validator.add_check({"Disk Space", HealthCategory::kData, false, "Low disk"});
    REQUIRE_FALSE(validator.is_healthy());
    REQUIRE(validator.failed_count() == 1);
}

TEST_CASE("SystemHealthValidator run and get results", "[phase49][health]")
{
    SystemHealthValidator validator;
    validator.add_check({"Test", HealthCategory::kCore, true, "OK"});
    validator.run_all_checks();
    auto results = validator.get_results();
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].passed);
}

TEST_CASE("SystemHealthValidator clear", "[phase49][health]")
{
    SystemHealthValidator validator;
    validator.load_default_checks();
    validator.clear();
    REQUIRE(validator.check_count() == 0);
}

// ============================================================================
// RegressionTestEngine
// ============================================================================

TEST_CASE("RegressionTestEngine initial state", "[phase49][regression]")
{
    RegressionTestEngine engine;
    REQUIRE(engine.get_results().empty());
}

TEST_CASE("RegressionTestEngine add baseline and pass", "[phase49][regression]")
{
    RegressionTestEngine engine;
    engine.add_baseline({"render_time", 100.0, 0.05}); // 5% tolerance
    REQUIRE(engine.has_baseline("render_time"));

    auto result = engine.run_regression("render_time", 103.0); // 3% delta
    REQUIRE(result.passed);
    REQUIRE(engine.passed_count() == 1);
}

TEST_CASE("RegressionTestEngine regression detected", "[phase49][regression]")
{
    RegressionTestEngine engine;
    engine.add_baseline({"render_time", 100.0, 0.05});

    auto result = engine.run_regression("render_time", 120.0); // 20% delta
    REQUIRE_FALSE(result.passed);
    REQUIRE(engine.failed_count() == 1);
}

TEST_CASE("RegressionTestEngine within_tolerance static", "[phase49][regression]")
{
    REQUIRE(RegressionTestEngine::within_tolerance(102.0, 100.0, 0.05));
    REQUIRE_FALSE(RegressionTestEngine::within_tolerance(120.0, 100.0, 0.05));
    REQUIRE(RegressionTestEngine::within_tolerance(0.0, 0.0, 0.01));
}

TEST_CASE("RegressionTestEngine clear", "[phase49][regression]")
{
    RegressionTestEngine engine;
    engine.add_baseline({"test", 100.0, 0.05});
    engine.run_regression("test", 100.0);
    engine.clear();
    REQUIRE(engine.get_results().empty());
    REQUIRE_FALSE(engine.has_baseline("test"));
}

// ============================================================================
// IntegrationTestCommandProvider
// ============================================================================

TEST_CASE("IntegrationTestCommandProvider provides 8 commands", "[phase49][commands]")
{
    REQUIRE(IntegrationTestCommandProvider::command_count() == 8);
    REQUIRE(IntegrationTestCommandProvider::command_ids().size() == 8);
}

TEST_CASE("IntegrationTestCommandProvider lookup", "[phase49][commands]")
{
    IntegrationTestCommandProvider provider;
    auto cmd = provider.get_command("integration.runAll");
    REQUIRE(cmd.id == "integration.runAll");
    REQUIRE(cmd.category == "Testing");
}

// ============================================================================
// ValidationCommandProvider
// ============================================================================

TEST_CASE("ValidationCommandProvider provides 8 commands", "[phase49][commands]")
{
    REQUIRE(ValidationCommandProvider::command_count() == 8);
    REQUIRE(ValidationCommandProvider::command_ids().size() == 8);
}

TEST_CASE("ValidationCommandProvider lookup", "[phase49][commands]")
{
    ValidationCommandProvider provider;
    auto cmd = provider.get_command("validate.runRegression");
    REQUIRE(cmd.id == "validate.runRegression");
    REQUIRE(cmd.category == "Validation");
}

// ============================================================================
// Phase 49 Events
// ============================================================================

TEST_CASE("TestSuiteCompletedEvent fields", "[phase49][events]")
{
    events::TestSuiteCompletedEvent evt;
    evt.suite_id = "suite_1";
    evt.passed = 10;
    evt.failed = 2;
    REQUIRE(evt.passed == 10);
}

TEST_CASE("TestFailedEvent fields", "[phase49][events]")
{
    events::TestFailedEvent evt;
    evt.test_id = "t1";
    evt.test_name = "Rendering";
    evt.error_msg = "Timeout";
    REQUIRE(evt.error_msg == "Timeout");
}

TEST_CASE("HealthCheckCompletedEvent fields", "[phase49][events]")
{
    events::HealthCheckCompletedEvent evt;
    evt.checks_passed = 9;
    evt.checks_failed = 1;
    evt.is_healthy = false;
    REQUIRE_FALSE(evt.is_healthy);
}

TEST_CASE("RegressionDetectedEvent fields", "[phase49][events]")
{
    events::RegressionDetectedEvent evt;
    evt.test_name = "render_time";
    evt.expected_val = 100.0;
    evt.actual_val = 150.0;
    REQUIRE(evt.actual_val == 150.0);
}

TEST_CASE("DependencyCheckEvent fields", "[phase49][events]")
{
    events::DependencyCheckEvent evt;
    evt.modules_checked = 5;
    evt.deps_satisfied = 4;
    evt.deps_missing = 1;
    REQUIRE(evt.deps_missing == 1);
}

TEST_CASE("IntegrationReportEvent fields", "[phase49][events]")
{
    events::IntegrationReportEvent evt;
    evt.total_tests = 100;
    evt.total_passed = 95;
    evt.total_failed = 5;
    REQUIRE(evt.total_failed == 5);
}
