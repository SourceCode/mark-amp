/// test_memory_budget.cpp — Phase 19: MemoryBudget tracker tests
///
/// Validates subsystem registration, usage reporting, over-budget detection,
/// and snapshot generation.

#include "core/MemoryBudget.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ══════════════════════════════════════════
// Registration & Reporting
// ══════════════════════════════════════════

TEST_CASE("MemoryBudget: register and report usage", "[memory_budget]")
{
    MemoryBudget budget;
    budget.register_subsystem("EventBus", 4 * 1024 * 1024); // 4MB
    budget.report("EventBus", 1 * 1024 * 1024);             // 1MB

    REQUIRE(budget.current_usage("EventBus") == 1 * 1024 * 1024);
    REQUIRE(budget.subsystem_count() == 1);
}

TEST_CASE("MemoryBudget: unregistered subsystem returns 0", "[memory_budget]")
{
    MemoryBudget budget;
    REQUIRE(budget.current_usage("NonExistent") == 0);
}

TEST_CASE("MemoryBudget: report to unregistered subsystem is ignored", "[memory_budget]")
{
    MemoryBudget budget;
    budget.report("Ghost", 999); // no-op
    REQUIRE(budget.current_usage("Ghost") == 0);
}

// ══════════════════════════════════════════
// Over-budget Detection
// ══════════════════════════════════════════

TEST_CASE("MemoryBudget: under budget returns false", "[memory_budget]")
{
    MemoryBudget budget;
    budget.register_subsystem("Cache", 1024);
    budget.report("Cache", 512);

    REQUIRE_FALSE(budget.is_over_budget("Cache"));
}

TEST_CASE("MemoryBudget: at budget returns false", "[memory_budget]")
{
    MemoryBudget budget;
    budget.register_subsystem("Cache", 1024);
    budget.report("Cache", 1024);

    REQUIRE_FALSE(budget.is_over_budget("Cache"));
}

TEST_CASE("MemoryBudget: over budget returns true", "[memory_budget]")
{
    MemoryBudget budget;
    budget.register_subsystem("Cache", 1024);
    budget.report("Cache", 2048);

    REQUIRE(budget.is_over_budget("Cache"));
}

TEST_CASE("MemoryBudget: unregistered subsystem not over budget", "[memory_budget]")
{
    MemoryBudget budget;
    REQUIRE_FALSE(budget.is_over_budget("NonExistent"));
}

// ══════════════════════════════════════════
// Snapshot
// ══════════════════════════════════════════

TEST_CASE("MemoryBudget: snapshot returns all subsystems", "[memory_budget]")
{
    MemoryBudget budget;
    budget.register_subsystem("EventBus", 4000);
    budget.register_subsystem("OutputChannel", 8000);
    budget.report("EventBus", 2000);
    budget.report("OutputChannel", 7000);

    auto snap = budget.snapshot();
    REQUIRE(snap.size() == 2);

    // Find EventBus in snapshot
    bool found_event_bus = false;
    bool found_output_channel = false;
    for (const auto& entry : snap)
    {
        if (entry.name == "EventBus")
        {
            found_event_bus = true;
            REQUIRE(entry.budget_bytes == 4000);
            REQUIRE(entry.current_bytes == 2000);
            REQUIRE(entry.percentage() == 50.0);
            REQUIRE_FALSE(entry.is_over_budget());
        }
        if (entry.name == "OutputChannel")
        {
            found_output_channel = true;
            REQUIRE(entry.budget_bytes == 8000);
            REQUIRE(entry.current_bytes == 7000);
        }
    }
    REQUIRE(found_event_bus);
    REQUIRE(found_output_channel);
}

// ══════════════════════════════════════════
// SubsystemUsage struct
// ══════════════════════════════════════════

TEST_CASE("SubsystemUsage: percentage calculation", "[memory_budget]")
{
    SubsystemUsage usage{"Test", 1000, 250};
    REQUIRE(usage.percentage() == 25.0);
}

TEST_CASE("SubsystemUsage: zero budget percentage is 0", "[memory_budget]")
{
    SubsystemUsage usage{"Test", 0, 100};
    REQUIRE(usage.percentage() == 0.0);
}

// ══════════════════════════════════════════
// Multiple Subsystems
// ══════════════════════════════════════════

TEST_CASE("MemoryBudget: subsystems are independent", "[memory_budget]")
{
    MemoryBudget budget;
    budget.register_subsystem("Alpha", 1000);
    budget.register_subsystem("Beta", 2000);

    budget.report("Alpha", 1500); // over
    budget.report("Beta", 500);   // under

    REQUIRE(budget.is_over_budget("Alpha"));
    REQUIRE_FALSE(budget.is_over_budget("Beta"));
}

// ══════════════════════════════════════════
// Reset
// ══════════════════════════════════════════

TEST_CASE("MemoryBudget: reset_usage clears all usage but keeps budgets", "[memory_budget]")
{
    MemoryBudget budget;
    budget.register_subsystem("EventBus", 4000);
    budget.report("EventBus", 3000);

    budget.reset_usage();

    REQUIRE(budget.current_usage("EventBus") == 0);
    REQUIRE_FALSE(budget.is_over_budget("EventBus"));

    // Budget should still be registered
    REQUIRE(budget.subsystem_count() == 1);
}
