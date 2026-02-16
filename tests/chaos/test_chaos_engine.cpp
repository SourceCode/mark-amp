/// test_chaos_engine.cpp — Phase 37: ChaosEngine tests

#include "core/ChaosEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ChaosEngine: starts disabled", "[chaos_engine]")
{
    ChaosEngine engine;
    REQUIRE_FALSE(engine.is_enabled());
}

TEST_CASE("ChaosEngine: no injection when disabled", "[chaos_engine]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::Delay, 1.0, "*", 10, true}); // 100% probability

    auto result = engine.check("anything");
    REQUIRE_FALSE(result.should_inject);
}

TEST_CASE("ChaosEngine: injection when enabled with 100% probability", "[chaos_engine]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::Delay, 1.0, "*", 5, true});
    engine.enable();

    auto result = engine.check("render");
    REQUIRE(result.should_inject);
    REQUIRE(result.type == ChaosType::Delay);
    REQUIRE(result.param == 5);
}

TEST_CASE("ChaosEngine: targeted rule only matches subsystem", "[chaos_engine]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::Delay, 1.0, "render", 1, true});
    engine.enable();

    auto result_hit = engine.check("render");
    REQUIRE(result_hit.should_inject);

    auto result_miss = engine.check("parser");
    REQUIRE_FALSE(result_miss.should_inject);
}

TEST_CASE("ChaosEngine: injection count tracking", "[chaos_engine]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::EventDrop, 1.0, "*", 0, true});
    engine.enable();

    engine.check("a");
    engine.check("b");
    engine.check("a");

    REQUIRE(engine.injection_count() == 3);

    auto& per_sub = engine.subsystem_injections();
    REQUIRE(per_sub.at("a") == 2);
    REQUIRE(per_sub.at("b") == 1);
}

TEST_CASE("ChaosEngine: disabled rule is skipped", "[chaos_engine]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::Delay, 1.0, "*", 1, false}); // disabled
    engine.enable();

    auto result = engine.check("test");
    REQUIRE_FALSE(result.should_inject);
}

TEST_CASE("ChaosEngine: rule_count", "[chaos_engine]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::Delay, 0.5, "*", 1, true});
    engine.add_rule({ChaosType::EventDrop, 0.1, "bus", 0, true});

    REQUIRE(engine.rule_count() == 2);
}

TEST_CASE("ChaosEngine: reset_counters", "[chaos_engine]")
{
    ChaosEngine engine;
    engine.add_rule({ChaosType::Delay, 1.0, "*", 1, true});
    engine.enable();
    engine.check("test");

    REQUIRE(engine.injection_count() == 1);

    engine.reset_counters();
    REQUIRE(engine.injection_count() == 0);
    REQUIRE(engine.subsystem_injections().empty());
}
