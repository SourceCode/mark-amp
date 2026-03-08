// test_frame_budget_enforcer.cpp — 10 tests for FrameBudgetEnforcer
#include "core/FrameBudgetEnforcer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("FrameBudgetEnforcer starts with zero frames", "[budget_enforcer]")
{
    FrameBudgetEnforcer enforcer;
    CHECK(enforcer.total_frames() == 0);
    CHECK(enforcer.overrun_frames() == 0);
    CHECK(enforcer.consecutive_overruns() == 0);
}

TEST_CASE("FrameBudgetEnforcer register_subsystem increases count", "[budget_enforcer]")
{
    FrameBudgetEnforcer enforcer;
    enforcer.register_subsystem("Test", [](const FrameBudgetToken&) {});
    CHECK(enforcer.subsystem_count() == 1);
}

TEST_CASE("FrameBudgetEnforcer run_frame returns stats", "[budget_enforcer]")
{
    FrameBudgetEnforcer enforcer;
    enforcer.register_subsystem("Noop", [](const FrameBudgetToken&) {});
    auto stats = enforcer.run_frame();
    CHECK(stats.budget.count() > 0);
    CHECK(enforcer.total_frames() == 1);
}

TEST_CASE("FrameBudgetEnforcer runs subsystems in order", "[budget_enforcer]")
{
    FrameBudgetEnforcer enforcer;
    std::vector<int> order;
    enforcer.register_subsystem("First", [&order](const FrameBudgetToken&) { order.push_back(1); });
    enforcer.register_subsystem("Second",
                                [&order](const FrameBudgetToken&) { order.push_back(2); });
    enforcer.run_frame();
    REQUIRE(order.size() == 2);
    CHECK(order[0] == 1);
    CHECK(order[1] == 2);
}

TEST_CASE("FrameBudgetEnforcer overrun percentage starts at zero", "[budget_enforcer]")
{
    FrameBudgetEnforcer enforcer;
    CHECK(enforcer.overrun_percentage() == 0.0);
}

TEST_CASE("FrameBudgetEnforcer on_input_activity switches mode", "[budget_enforcer]")
{
    FrameBudgetEnforcer enforcer;
    enforcer.on_input_activity();
    auto& throttle = enforcer.throttle();
    CHECK(throttle.is_typing());
}

TEST_CASE("FrameBudgetEnforcer kMaxConsecutiveOverruns is 5", "[budget_enforcer]")
{
    CHECK(FrameBudgetEnforcer::kMaxConsecutiveOverruns == 5);
}

TEST_CASE("FrameBudgetEnforcer is_sustained_overrun false initially", "[budget_enforcer]")
{
    FrameBudgetEnforcer enforcer;
    CHECK_FALSE(enforcer.is_sustained_overrun());
}

TEST_CASE("FrameBudgetEnforcer empty subsystems still increments frame", "[budget_enforcer]")
{
    FrameBudgetEnforcer enforcer;
    enforcer.run_frame();
    enforcer.run_frame();
    CHECK(enforcer.total_frames() == 2);
}

TEST_CASE("FrameBudgetEnforcer activity mode reflects in stats", "[budget_enforcer]")
{
    FrameBudgetEnforcer enforcer;
    auto stats = enforcer.run_frame();
    CHECK(stats.activity == ActivityMode::Idle);
}
