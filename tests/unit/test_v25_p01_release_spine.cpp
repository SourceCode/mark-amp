/// @file test_v25_p01_release_spine.cpp
/// @brief V25 Phase 01: Release spine, ledger bootstrap, gate enforcement tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25LedgerBootstrap.h"
#include "core/ReleaseGateEnforcer.h"
#include "core/EventBus.h"

using namespace markamp::core;

TEST_CASE("V25 P01: Ledger bootstrap registers all 60 tasks", "[v25][p01]")
{
    ExecutionLedger ledger;
    bootstrap_v25_tasks(ledger);
    REQUIRE(ledger.task_count() >= 50);
}

TEST_CASE("V25 P01: Criteria bootstrap registers V25 criteria", "[v25][p01]")
{
    SubsystemDoneCriteria criteria;
    bootstrap_v25_criteria(criteria);
    REQUIRE(criteria.total_criteria() > 0);
}

TEST_CASE("V25 P01: Release gate enforcer blocks on dead actions", "[v25][p01]")
{
    ActionReadinessGate gate;
    CompletionInventory inventory;
    SubsystemDoneCriteria criteria;

    ActionReadinessItem dead_action;
    dead_action.action_id = "test.dead";
    dead_action.status = ActionBindingStatus::kDead;
    gate.add_action(dead_action);

    ReleaseGateEnforcer enforcer;
    auto verdict = enforcer.enforce(gate, inventory, criteria);
    REQUIRE_FALSE(verdict.passes);
    REQUIRE(verdict.dead_action_count > 0);
}

TEST_CASE("V25 P01: Release gate passes when clean", "[v25][p01]")
{
    ActionReadinessGate gate;
    CompletionInventory inventory;
    SubsystemDoneCriteria criteria;

    ActionReadinessItem live_action;
    live_action.action_id = "test.live";
    live_action.status = ActionBindingStatus::kLive;
    gate.add_action(live_action);

    ReleaseGateEnforcer enforcer;
    auto verdict = enforcer.enforce(gate, inventory, criteria);
    REQUIRE(verdict.passes);
}

TEST_CASE("V25 P01: Release gate verdict export", "[v25][p01]")
{
    ReleaseGateVerdict v;
    v.passes = true;
    auto md = ReleaseGateEnforcer::export_markdown(v);
    REQUIRE(md.find("PASS") != std::string::npos);
}

TEST_CASE("V25 P01: Ledger tasks have valid IDs and phases", "[v25][p01]")
{
    ExecutionLedger ledger;
    bootstrap_v25_tasks(ledger);

    // Verify first and last phase tasks exist
    auto* first = ledger.get_task("P01-T01");
    REQUIRE(first != nullptr);
    REQUIRE_FALSE(first->task_id.empty());
    REQUIRE_FALSE(first->phase_id.empty());
    REQUIRE_FALSE(first->title.empty());

    auto* last = ledger.get_task("P20-T03");
    REQUIRE(last != nullptr);
    REQUIRE_FALSE(last->task_id.empty());
}

TEST_CASE("V25 P01: Phase count is 20", "[v25][p01]")
{
    REQUIRE(v25_phase_count() == 20);
}
