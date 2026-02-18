/// @file DataRedactionEngine.cpp
/// @brief V9 Phase 44 — DataRedactionEngine implementation.

#include "DataRedactionEngine.h"

#include <algorithm>

namespace markamp::core
{

auto redaction_rule_type_name(RedactionRuleType rule_type) -> std::string
{
    switch (rule_type)
    {
        case RedactionRuleType::kEmail:
            return "email";
        case RedactionRuleType::kPhone:
            return "phone";
        case RedactionRuleType::kSsn:
            return "ssn";
        case RedactionRuleType::kCreditCard:
            return "credit_card";
        case RedactionRuleType::kIpAddress:
            return "ip_address";
        case RedactionRuleType::kCustomRegex:
            return "custom_regex";
    }
    return "unknown";
}

void DataRedactionEngine::add_rule(RedactionRule rule)
{
    if (rule.rule_id.empty())
    {
        rule.rule_id = "rule_" + std::to_string(next_id_++);
    }
    rules_.push_back(std::move(rule));
}

auto DataRedactionEngine::remove_rule(const std::string& rule_id) -> bool
{
    auto iter = std::remove_if(rules_.begin(),
                               rules_.end(),
                               [&](const RedactionRule& rule) { return rule.rule_id == rule_id; });
    if (iter == rules_.end())
    {
        return false;
    }
    rules_.erase(iter, rules_.end());
    return true;
}

auto DataRedactionEngine::find_rule(const std::string& rule_id) const -> const RedactionRule*
{
    for (const auto& rule : rules_)
    {
        if (rule.rule_id == rule_id)
        {
            return &rule;
        }
    }
    return nullptr;
}

void DataRedactionEngine::load_defaults()
{
    RedactionRule email;
    email.rule_id = "default_email";
    email.pattern = "@";
    email.replacement = "[EMAIL REDACTED]";
    email.rule_type = RedactionRuleType::kEmail;
    rules_.push_back(std::move(email));

    RedactionRule phone;
    phone.rule_id = "default_phone";
    phone.pattern = "555-";
    phone.replacement = "[PHONE REDACTED]";
    phone.rule_type = RedactionRuleType::kPhone;
    rules_.push_back(std::move(phone));

    RedactionRule ssn;
    ssn.rule_id = "default_ssn";
    ssn.pattern = "SSN:";
    ssn.replacement = "[SSN REDACTED]";
    ssn.rule_type = RedactionRuleType::kSsn;
    rules_.push_back(std::move(ssn));

    RedactionRule credit_card;
    credit_card.rule_id = "default_cc";
    credit_card.pattern = "4111-";
    credit_card.replacement = "[CC REDACTED]";
    credit_card.rule_type = RedactionRuleType::kCreditCard;
    rules_.push_back(std::move(credit_card));

    RedactionRule ip_addr;
    ip_addr.rule_id = "default_ip";
    ip_addr.pattern = "192.168.";
    ip_addr.replacement = "[IP REDACTED]";
    ip_addr.rule_type = RedactionRuleType::kIpAddress;
    rules_.push_back(std::move(ip_addr));
}

auto DataRedactionEngine::redact(const std::string& content) const -> RedactionResult
{
    RedactionResult result;
    result.original_length = static_cast<int>(content.size());
    std::string current = content;
    int total_redactions = 0;
    int rules_hit = 0;

    for (const auto& rule : rules_)
    {
        if (!rule.is_active)
        {
            continue;
        }
        auto [redacted, count] = apply_rule(current, rule);
        if (count > 0)
        {
            current = std::move(redacted);
            total_redactions += count;
            ++rules_hit;
        }
    }

    result.redacted_text = std::move(current);
    result.redacted_length = static_cast<int>(result.redacted_text.size());
    result.redactions_applied = total_redactions;
    result.rules_matched = rules_hit;
    return result;
}

auto DataRedactionEngine::scan(const std::string& content) const -> std::vector<PiiMatch>
{
    std::vector<PiiMatch> matches;
    for (const auto& rule : rules_)
    {
        if (!rule.is_active)
        {
            continue;
        }
        size_t pos = 0;
        while ((pos = content.find(rule.pattern, pos)) != std::string::npos)
        {
            PiiMatch match;
            match.matched_text = rule.pattern;
            match.rule_type = rule.rule_type;
            match.position = static_cast<int>(pos);
            match.length = static_cast<int>(rule.pattern.size());
            matches.push_back(std::move(match));
            pos += rule.pattern.size();
        }
    }
    return matches;
}

auto DataRedactionEngine::rule_count() const -> int
{
    return static_cast<int>(rules_.size());
}

auto DataRedactionEngine::active_rule_count() const -> int
{
    return static_cast<int>(std::count_if(
        rules_.begin(), rules_.end(), [](const RedactionRule& rule) { return rule.is_active; }));
}

void DataRedactionEngine::clear_rules()
{
    rules_.clear();
}

auto DataRedactionEngine::apply_rule(const std::string& content, const RedactionRule& rule) const
    -> std::pair<std::string, int>
{
    std::string result;
    int count = 0;
    size_t pos = 0;
    size_t prev = 0;

    while ((pos = content.find(rule.pattern, prev)) != std::string::npos)
    {
        result += content.substr(prev, pos - prev);
        result += rule.replacement;
        prev = pos + rule.pattern.size();
        ++count;
    }
    result += content.substr(prev);
    return {result, count};
}

} // namespace markamp::core
