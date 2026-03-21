/// @file test_v20_execution_scoreboard.cpp
/// @brief V20 Phase 10 – ExecutionScoreboard unit tests.

#include "core/ExecutionScoreboard.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("Scoreboard: defaults registered", "[v20][scoreboard]")
{
    EventBus bus;
    ExecutionScoreboard scoreboard(bus);

    REQUIRE(scoreboard.phase_count() == 10);
    REQUIRE(scoreboard.all_exit_criteria().size() >= 7);
}

TEST_CASE("Scoreboard: phase lookup", "[v20][scoreboard]")
{
    EventBus bus;
    ExecutionScoreboard scoreboard(bus);

    auto* p01 = scoreboard.phase("P01");
    REQUIRE(p01 != nullptr);
    REQUIRE(p01->phase_name == "Artifact Creation Spine & Shell Ownership");
    REQUIRE(p01->tasks_total == 6);

    REQUIRE(scoreboard.phase("nonexistent") == nullptr);
}

TEST_CASE("Scoreboard: update phase progress", "[v20][scoreboard]")
{
    EventBus bus;
    ExecutionScoreboard scoreboard(bus);

    PhaseProgress progress;
    progress.phase_id = "P01";
    progress.phase_name = "Artifact Creation Spine & Shell Ownership";
    progress.status = PhaseStatus::kComplete;
    progress.tasks_total = 6;
    progress.tasks_complete = 6;
    progress.improvements_total = 210;
    scoreboard.set_phase(progress);

    auto* p = scoreboard.phase("P01");
    REQUIRE(p->is_complete());
    REQUIRE(p->completion_pct() == 100.0);
}

TEST_CASE("Scoreboard: performance budgets", "[v20][scoreboard]")
{
    EventBus bus;
    ExecutionScoreboard scoreboard(bus);

    scoreboard.record_performance("file.create", 50.0, 100.0);
    scoreboard.record_performance("file.save", 150.0, 100.0);

    auto perf = scoreboard.all_performance();
    REQUIRE(perf.size() == 2);
    REQUIRE(perf[0].within_budget);
    REQUIRE_FALSE(perf[1].within_budget);
    REQUIRE(perf[1].exceeded());
}

TEST_CASE("Scoreboard: exit criteria", "[v20][scoreboard]")
{
    EventBus bus;
    ExecutionScoreboard scoreboard(bus);

    // Initially, no criteria are met
    REQUIRE(scoreboard.unmet_criteria_count() >= 7);
    REQUIRE_FALSE(scoreboard.ready_for_exit());

    // Satisfy all criteria
    for (auto& ec : scoreboard.all_exit_criteria())
    {
        ec.met = true;
        ec.evidence = "smoke tests pass";
        scoreboard.set_exit_criterion(ec);
    }

    REQUIRE(scoreboard.unmet_criteria_count() == 0);
    REQUIRE(scoreboard.ready_for_exit());
}

TEST_CASE("Scoreboard: overall completion", "[v20][scoreboard]")
{
    EventBus bus;
    ExecutionScoreboard scoreboard(bus);

    // Initially all phases are 0%
    REQUIRE(scoreboard.overall_completion() == 0.0);

    // Complete one phase
    PhaseProgress p;
    p.phase_id = "P01";
    p.phase_name = "test";
    p.tasks_total = 6;
    p.tasks_complete = 6;
    p.status = PhaseStatus::kComplete;
    scoreboard.set_phase(p);

    REQUIRE(scoreboard.overall_completion() > 0.0);
}
