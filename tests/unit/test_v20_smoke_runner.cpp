/// @file test_v20_smoke_runner.cpp
/// @brief V20 Phase 10 – WorkflowSmokeRunner unit tests.

#include "core/WorkflowSmokeRunner.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SmokeRunner: construction", "[v20][smoke-runner]")
{
    EventBus bus;
    WorkflowSmokeRunner runner(bus);
    REQUIRE(runner.test_count() == 0);
    REQUIRE(runner.pass_rate() == 0.0);
}

TEST_CASE("SmokeRunner: register and run passing test", "[v20][smoke-runner]")
{
    EventBus bus;
    WorkflowSmokeRunner runner(bus);

    runner.register_test("smoke.file.create", "New file creation", []() {
        SmokeTestResult r;
        r.status = SmokeTestStatus::kPassed;
        return r;
    });

    REQUIRE(runner.test_count() == 1);

    auto results = runner.run_all();
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].passed());
    REQUIRE(runner.pass_rate() == 1.0);
}

TEST_CASE("SmokeRunner: mixed pass/fail", "[v20][smoke-runner]")
{
    EventBus bus;
    WorkflowSmokeRunner runner(bus);

    runner.register_test("smoke.pass", "Passing test", []() {
        SmokeTestResult r;
        r.status = SmokeTestStatus::kPassed;
        return r;
    });
    runner.register_test("smoke.fail", "Failing test", []() {
        SmokeTestResult r;
        r.status = SmokeTestStatus::kFailed;
        r.error_message = "Expected behavior not observed";
        return r;
    });

    auto results = runner.run_all();
    REQUIRE(results.size() == 2);
    REQUIRE(runner.pass_rate() == 0.5);
}

TEST_CASE("SmokeRunner: run specific test", "[v20][smoke-runner]")
{
    EventBus bus;
    WorkflowSmokeRunner runner(bus);

    runner.register_test("smoke.a", "Test A", []() {
        SmokeTestResult r;
        r.status = SmokeTestStatus::kPassed;
        return r;
    });

    auto result = runner.run_test("smoke.a");
    REQUIRE(result.passed());
    REQUIRE(result.duration_ms >= 0.0);

    // Non-existent test
    auto missing = runner.run_test("smoke.missing");
    REQUIRE_FALSE(missing.passed());
}

TEST_CASE("SmokeRunner: legacy pathway tracking", "[v20][smoke-runner]")
{
    EventBus bus;
    WorkflowSmokeRunner runner(bus);

    LegacyPathway legacy;
    legacy.pathway_id = "mainframe.untitled";
    legacy.file_path = "src/ui/MainFrame.cpp";
    legacy.description = "Direct untitled file creation";
    legacy.replacement = "ArtifactCreationService";
    runner.register_legacy_pathway(legacy);

    REQUIRE(runner.active_legacy_count() == 1);

    runner.retire_pathway("mainframe.untitled");
    REQUIRE(runner.active_legacy_count() == 0);
}

TEST_CASE("SmokeRunner: smoke event published", "[v20][smoke-runner]")
{
    EventBus bus;
    int event_count = 0;
    auto sub = bus.subscribe<events::SmokeRunCompletedEvent>(
        [&](const events::SmokeRunCompletedEvent& evt) {
            ++event_count;
            REQUIRE(evt.total_tests == 1);
            REQUIRE(evt.passed == 1);
        });

    WorkflowSmokeRunner runner(bus);
    runner.register_test("smoke.test", "Test", []() {
        SmokeTestResult r;
        r.status = SmokeTestStatus::kPassed;
        return r;
    });

    runner.run_all();
    REQUIRE(event_count == 1);
}
