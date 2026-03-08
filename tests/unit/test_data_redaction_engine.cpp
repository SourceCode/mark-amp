// test_data_redaction_engine.cpp — 10 tests for DataRedactionEngine
#include "core/DataRedactionEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("DataRedactionEngine starts with no rules", "[security][redaction]")
{
    DataRedactionEngine engine;
    CHECK(engine.rule_count() == 0);
    CHECK(engine.active_rule_count() == 0);
}

TEST_CASE("DataRedactionEngine add_rule increases count", "[security][redaction]")
{
    DataRedactionEngine engine;
    RedactionRule rule;
    rule.rule_id = "r1";
    rule.pattern = "SSN";
    rule.replacement = "[REDACTED]";
    rule.rule_type = RedactionRuleType::kSsn;
    engine.add_rule(rule);
    CHECK(engine.rule_count() == 1);
    CHECK(engine.active_rule_count() == 1);
}

TEST_CASE("DataRedactionEngine remove_rule", "[security][redaction]")
{
    DataRedactionEngine engine;
    RedactionRule rule;
    rule.rule_id = "r1";
    rule.pattern = "email";
    engine.add_rule(rule);
    CHECK(engine.remove_rule("r1"));
    CHECK(engine.rule_count() == 0);
}

TEST_CASE("DataRedactionEngine remove_rule nonexistent returns false", "[security][redaction]")
{
    DataRedactionEngine engine;
    CHECK_FALSE(engine.remove_rule("ghost"));
}

TEST_CASE("DataRedactionEngine find_rule", "[security][redaction]")
{
    DataRedactionEngine engine;
    RedactionRule rule;
    rule.rule_id = "find-me";
    rule.pattern = "test";
    engine.add_rule(rule);
    auto found = engine.find_rule("find-me");
    REQUIRE(found != nullptr);
    CHECK(found->pattern == "test");
}

TEST_CASE("DataRedactionEngine find_rule missing returns null", "[security][redaction]")
{
    DataRedactionEngine engine;
    CHECK(engine.find_rule("missing") == nullptr);
}

TEST_CASE("DataRedactionEngine redact applies rules", "[security][redaction]")
{
    DataRedactionEngine engine;
    RedactionRule rule;
    rule.rule_id = "r1";
    rule.pattern = "secret-word";
    rule.replacement = "[REDACTED]";
    engine.add_rule(rule);
    auto result = engine.redact("This contains secret-word in text");
    CHECK(result.redactions_applied > 0);
    CHECK(result.redacted_text.find("secret-word") == std::string::npos);
    CHECK(result.redacted_text.find("[REDACTED]") != std::string::npos);
}

TEST_CASE("DataRedactionEngine redact no match returns original", "[security][redaction]")
{
    DataRedactionEngine engine;
    RedactionRule rule;
    rule.rule_id = "r1";
    rule.pattern = "xyz-not-present";
    rule.replacement = "[REDACTED]";
    engine.add_rule(rule);
    auto result = engine.redact("Clean content here");
    CHECK(result.redactions_applied == 0);
    CHECK(result.redacted_text == "Clean content here");
}

TEST_CASE("DataRedactionEngine clear_rules removes all", "[security][redaction]")
{
    DataRedactionEngine engine;
    RedactionRule r1;
    r1.rule_id = "a";
    r1.pattern = "p1";
    RedactionRule r2;
    r2.rule_id = "b";
    r2.pattern = "p2";
    engine.add_rule(r1);
    engine.add_rule(r2);
    CHECK(engine.rule_count() == 2);
    engine.clear_rules();
    CHECK(engine.rule_count() == 0);
}

TEST_CASE("redaction_rule_type_name returns display names", "[security][redaction]")
{
    auto email = redaction_rule_type_name(RedactionRuleType::kEmail);
    auto phone = redaction_rule_type_name(RedactionRuleType::kPhone);
    auto ssn = redaction_rule_type_name(RedactionRuleType::kSsn);
    CHECK_FALSE(email.empty());
    CHECK_FALSE(phone.empty());
    CHECK_FALSE(ssn.empty());
    CHECK(email != phone);
}
