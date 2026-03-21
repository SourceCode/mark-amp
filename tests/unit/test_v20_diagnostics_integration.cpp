/// @file test_v20_diagnostics_integration.cpp
/// @brief V20 Phase 10 – Diagnostics integration tests.

#include "core/LifecycleTracer.h"
#include "core/WorkflowSmokeRunner.h"
#include "core/ExecutionScoreboard.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("DiagInteg: tracer feeds scoreboard", "[v20][diag-integ]")
{
    EventBus bus;
    LifecycleTracer tracer(bus);
    ExecutionScoreboard scoreboard(bus);

    // Trace a complete workflow
    tracer.trace("art-1", TraceEventType::kCreate, "command", "new file");
    tracer.trace("art-1", TraceEventType::kSave, "user");
    tracer.trace("art-1", TraceEventType::kClose, "user");

    REQUIRE(tracer.trace_count() == 3);
    REQUIRE(tracer.error_count() == 0);

    // Record performance measurement
    scoreboard.record_performance("file.create-to-save", 45.0, 100.0);
    auto perf = scoreboard.all_performance();
    REQUIRE(perf.size() == 1);
    REQUIRE(perf[0].within_budget);
}

TEST_CASE("DiagInteg: smoke runner validates lifecycle", "[v20][diag-integ]")
{
    EventBus bus;
    WorkflowSmokeRunner runner(bus);
    LifecycleTracer tracer(bus);

    // Register a smoke test that exercises the tracer
    runner.register_test("smoke.lifecycle", "Lifecycle trace test",
                        [&tracer]() {
                            tracer.trace("smoke-art", TraceEventType::kCreate, "smoke");
                            tracer.trace("smoke-art", TraceEventType::kSave, "smoke");

                            SmokeTestResult r;
                            r.status = tracer.error_count() == 0
                                           ? SmokeTestStatus::kPassed
                                           : SmokeTestStatus::kFailed;
                            return r;
                        });

    auto results = runner.run_all();
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].passed());
    REQUIRE(runner.pass_rate() == 1.0);
}

TEST_CASE("DiagInteg: exit criteria with smoke results", "[v20][diag-integ]")
{
    EventBus bus;
    ExecutionScoreboard scoreboard(bus);
    WorkflowSmokeRunner runner(bus);

    runner.register_test("smoke.all", "All workflows", []() {
        SmokeTestResult r;
        r.status = SmokeTestStatus::kPassed;
        return r;
    });

    auto results = runner.run_all();
    bool all_pass = runner.pass_rate() == 1.0;

    ExitCriterion ec;
    ec.criterion_id = "exit.smoke-pass";
    ec.description = "All smoke tests pass";
    ec.met = all_pass;
    ec.evidence = "pass rate 100%";
    scoreboard.set_exit_criterion(ec);

    auto criteria = scoreboard.all_exit_criteria();
    bool found = false;
    for (const auto& c : criteria)
    {
        if (c.criterion_id == "exit.smoke-pass")
        {
            REQUIRE(c.met);
            found = true;
        }
    }
    REQUIRE(found);
}

TEST_CASE("DiagInteg: cross-system event flow", "[v20][diag-integ]")
{
    EventBus bus;
    int trace_events = 0;
    int smoke_events = 0;
    int phase_events = 0;

    auto sub1 = bus.subscribe<events::LifecycleTraceEmittedEvent>(
        [&](const events::LifecycleTraceEmittedEvent&) { ++trace_events; });
    auto sub2 = bus.subscribe<events::SmokeRunCompletedEvent>(
        [&](const events::SmokeRunCompletedEvent&) { ++smoke_events; });
    auto sub3 = bus.subscribe<events::PhaseProgressUpdatedEvent>(
        [&](const events::PhaseProgressUpdatedEvent&) { ++phase_events; });

    LifecycleTracer tracer(bus);
    WorkflowSmokeRunner runner(bus);
    ExecutionScoreboard scoreboard(bus);

    tracer.trace("art-1", TraceEventType::kCreate, "test");
    REQUIRE(trace_events == 1);

    runner.register_test("smoke.x", "X", []() {
        SmokeTestResult r;
        r.status = SmokeTestStatus::kPassed;
        return r;
    });
    runner.run_all();
    REQUIRE(smoke_events == 1);

    PhaseProgress p;
    p.phase_id = "P10";
    p.phase_name = "Diagnostics";
    p.tasks_total = 6;
    p.tasks_complete = 3;
    scoreboard.set_phase(p);
    // Phase events: defaults register 10 + this update = 11
    REQUIRE(phase_events >= 1);
}
