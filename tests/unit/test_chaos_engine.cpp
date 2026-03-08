/// test_chaos_engine.cpp — Comprehensive tests for ChaosEngine
#include "core/ChaosEngine.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("ChaosEngine: default is disabled", "[chaos_engine][positive]")
{
    ChaosEngine engine;
    REQUIRE_FALSE(engine.is_enabled());
}

TEST_CASE("ChaosEngine: enable and disable", "[chaos_engine][positive]")
{
    ChaosEngine engine;
    engine.enable();
    REQUIRE(engine.is_enabled());
    engine.disable();
    REQUIRE_FALSE(engine.is_enabled());
}

TEST_CASE("ChaosEngine: add_rule increases rule count", "[chaos_engine][positive]")
{
    ChaosEngine engine;
    REQUIRE(engine.rule_count() == 0);
    engine.add_rule({ChaosType::Delay, 0.5, "render", 10, true});
    REQUIRE(engine.rule_count() == 1);
    engine.add_rule({ChaosType::EventDrop, 0.3, "*", 0, true});
    REQUIRE(engine.rule_count() == 2);
}

TEST_CASE("ChaosEngine: check with probability 1.0 always injects", "[chaos_engine][positive]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::AllocationFailure, 1.0, "*", 0, true});
    engine.enable();
    auto result = engine.check("anything");
    REQUIRE(result.should_inject);
    REQUIRE(result.type == ChaosType::AllocationFailure);
}

TEST_CASE("ChaosEngine: injection_count tracks injections", "[chaos_engine][positive]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::Delay, 1.0, "*", 5, true});
    engine.enable();
    REQUIRE(engine.injection_count() == 0);
    engine.check("sub1");
    REQUIRE(engine.injection_count() == 1);
    engine.check("sub2");
    REQUIRE(engine.injection_count() == 2);
}

TEST_CASE("ChaosEngine: subsystem_injections tracks per-subsystem", "[chaos_engine][positive]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::EventDrop, 1.0, "*", 0, true});
    engine.enable();
    engine.check("render");
    engine.check("render");
    engine.check("parser");
    auto& subs = engine.subsystem_injections();
    REQUIRE(subs.at("render") == 2);
    REQUIRE(subs.at("parser") == 1);
}

TEST_CASE("ChaosEngine: reset_counters clears counts", "[chaos_engine][positive]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::Delay, 1.0, "*", 0, true});
    engine.enable();
    engine.check("sub1");
    engine.reset_counters();
    REQUIRE(engine.injection_count() == 0);
    REQUIRE(engine.subsystem_injections().empty());
}

// ── Negative Tests ──

TEST_CASE("ChaosEngine: check when disabled never injects", "[chaos_engine][negative]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::Delay, 1.0, "*", 10, true});
    // Not enabled
    auto result = engine.check("render");
    REQUIRE_FALSE(result.should_inject);
}

TEST_CASE("ChaosEngine: check with probability 0.0 never injects", "[chaos_engine][negative]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::Delay, 0.0, "*", 10, true});
    engine.enable();
    // Run 20 times — none should inject
    for (int i = 0; i < 20; ++i)
    {
        auto result = engine.check("sub");
        REQUIRE_FALSE(result.should_inject);
    }
}

TEST_CASE("ChaosEngine: disabled rule does not inject", "[chaos_engine][negative]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::Delay, 1.0, "*", 10, false}); // disabled rule
    engine.enable();
    auto result = engine.check("sub");
    REQUIRE_FALSE(result.should_inject);
}

TEST_CASE("ChaosEngine: targeted rule only matches its target", "[chaos_engine][negative]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::Delay, 1.0, "render", 10, true}); // only "render"
    engine.enable();
    auto result = engine.check("parser"); // Different subsystem
    REQUIRE_FALSE(result.should_inject);
}

// ── Edge Cases ──

TEST_CASE("ChaosEngine: no rules returns no injection", "[chaos_engine][edge]")
{
    ChaosEngine engine;
    engine.enable();
    auto result = engine.check("anything");
    REQUIRE_FALSE(result.should_inject);
}

TEST_CASE("ChaosType: enum values exist", "[chaos_engine][edge]")
{
    REQUIRE(ChaosType::AllocationFailure != ChaosType::EventDrop);
    REQUIRE(ChaosType::Delay != ChaosType::ExceptionThrow);
}

TEST_CASE("ChaosRule: default values", "[chaos_engine][edge]")
{
    ChaosRule rule;
    REQUIRE(rule.type == ChaosType::Delay);
    REQUIRE(rule.probability == 0.0);
    REQUIRE(rule.target.empty());
    REQUIRE(rule.enabled == true);
}

TEST_CASE("ChaosCheckResult: default is not injecting", "[chaos_engine][edge]")
{
    ChaosCheckResult result;
    REQUIRE_FALSE(result.should_inject);
}
