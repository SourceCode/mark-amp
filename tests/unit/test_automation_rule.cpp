// test_automation_rule.cpp — 10 tests for AutomationRuleManager
#include "core/AutomationRule.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AutomationRuleManager starts empty", "[automation]")
{
    AutomationRuleManager mgr;
    CHECK(mgr.rule_count() == 0);
}

TEST_CASE("AutomationRuleManager add_rule increases count", "[automation]")
{
    AutomationRuleManager mgr;
    mgr.add_rule(
        {.rule_id = "r1", .name = "Auto-save lint", .trigger = AutomationTrigger::kOnFileSave});
    CHECK(mgr.rule_count() == 1);
}

TEST_CASE("AutomationRuleManager find_rule returns pointer", "[automation]")
{
    AutomationRuleManager mgr;
    mgr.add_rule({.rule_id = "r1", .name = "Rule 1"});
    auto* rule = mgr.find_rule("r1");
    REQUIRE(rule != nullptr);
    CHECK(rule->name == "Rule 1");
}

TEST_CASE("AutomationRuleManager find_rule returns nullptr for unknown", "[automation]")
{
    AutomationRuleManager mgr;
    CHECK(mgr.find_rule("nonexistent") == nullptr);
}

TEST_CASE("AutomationRuleManager remove_rule works", "[automation]")
{
    AutomationRuleManager mgr;
    mgr.add_rule({.rule_id = "r1", .name = "Rule 1"});
    CHECK(mgr.remove_rule("r1"));
    CHECK(mgr.rule_count() == 0);
    CHECK_FALSE(mgr.remove_rule("r1"));
}

TEST_CASE("AutomationRuleManager enable/disable_rule works", "[automation]")
{
    AutomationRuleManager mgr;
    mgr.add_rule({.rule_id = "r1", .name = "Rule 1", .enabled = true});
    CHECK(mgr.disable_rule("r1"));
    auto* rule = mgr.find_rule("r1");
    REQUIRE(rule != nullptr);
    CHECK_FALSE(rule->enabled);
    CHECK(mgr.enable_rule("r1"));
}

TEST_CASE("AutomationRuleManager rules_for_trigger filters", "[automation]")
{
    AutomationRuleManager mgr;
    mgr.add_rule({.rule_id = "r1", .trigger = AutomationTrigger::kOnFileSave});
    mgr.add_rule({.rule_id = "r2", .trigger = AutomationTrigger::kOnManual});
    mgr.add_rule({.rule_id = "r3", .trigger = AutomationTrigger::kOnFileSave});
    auto save_rules = mgr.rules_for_trigger(AutomationTrigger::kOnFileSave);
    CHECK(save_rules.size() == 2);
}

TEST_CASE("AutomationRuleManager mark_executed increments count", "[automation]")
{
    AutomationRuleManager mgr;
    mgr.add_rule({.rule_id = "r1"});
    CHECK(mgr.mark_executed("r1"));
    auto* rule = mgr.find_rule("r1");
    CHECK(rule->execution_count == 1);
}

TEST_CASE("AutomationRuleManager clear_all removes everything", "[automation]")
{
    AutomationRuleManager mgr;
    mgr.add_rule({.rule_id = "r1"});
    mgr.add_rule({.rule_id = "r2"});
    mgr.clear_all();
    CHECK(mgr.rule_count() == 0);
}

TEST_CASE("automation_trigger_name returns readable name", "[automation]")
{
    CHECK_FALSE(automation_trigger_name(AutomationTrigger::kOnFileSave).empty());
    CHECK_FALSE(automation_trigger_name(AutomationTrigger::kOnManual).empty());
    CHECK_FALSE(automation_action_name(AutomationAction::kRunCommand).empty());
}
