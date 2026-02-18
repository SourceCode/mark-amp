/// @file NotificationFilter.cpp
/// @brief V9 Phase 39 — NotificationFilter implementation.

#include "NotificationFilter.h"

#include <algorithm>

namespace markamp::core
{

void NotificationFilter::add_rule(NotificationFilterRule rule)
{
    if (rule.rule_id.empty())
    {
        rule.rule_id = "rule_" + std::to_string(rules_.size());
    }
    rules_.push_back(std::move(rule));
}

auto NotificationFilter::remove_rule(const std::string& rule_id) -> bool
{
    auto iter =
        std::remove_if(rules_.begin(),
                       rules_.end(),
                       [&](const NotificationFilterRule& rule) { return rule.rule_id == rule_id; });
    if (iter == rules_.end())
    {
        return false;
    }
    rules_.erase(iter, rules_.end());
    return true;
}

auto NotificationFilter::find_rule(const std::string& rule_id) const
    -> const NotificationFilterRule*
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

auto NotificationFilter::all_rules() const -> const std::vector<NotificationFilterRule>&
{
    return rules_;
}

auto NotificationFilter::rule_count() const -> int
{
    return static_cast<int>(rules_.size());
}

auto NotificationFilter::enable_rule(const std::string& rule_id) -> bool
{
    for (auto& rule : rules_)
    {
        if (rule.rule_id == rule_id)
        {
            rule.enabled = true;
            return true;
        }
    }
    return false;
}

auto NotificationFilter::disable_rule(const std::string& rule_id) -> bool
{
    for (auto& rule : rules_)
    {
        if (rule.rule_id == rule_id)
        {
            rule.enabled = false;
            return true;
        }
    }
    return false;
}

auto NotificationFilter::should_show(const CenterNotification& notification) const -> bool
{
    bool has_include_rules = false;
    bool matched_include = false;

    for (const auto& rule : rules_)
    {
        if (!rule.enabled)
        {
            continue;
        }

        if (rule.mode == FilterMatchMode::kExclude && matches_rule(notification, rule))
        {
            return false; // Excluded
        }

        if (rule.mode == FilterMatchMode::kInclude)
        {
            has_include_rules = true;
            if (matches_rule(notification, rule))
            {
                matched_include = true;
            }
        }
    }

    // If there are include rules, notification must match at least one
    if (has_include_rules)
    {
        return matched_include;
    }
    return true; // No include rules = show everything not excluded
}

auto NotificationFilter::apply_filters(const std::vector<const CenterNotification*>& notifications)
    const -> std::vector<const CenterNotification*>
{
    std::vector<const CenterNotification*> result;
    for (const auto* notif : notifications)
    {
        if (notif != nullptr && should_show(*notif))
        {
            result.push_back(notif);
        }
    }
    return result;
}

void NotificationFilter::load_defaults()
{
    rules_.clear();

    // Default: show high priority and above
    NotificationFilterRule high_priority;
    high_priority.rule_id = "default_high_priority";
    high_priority.description = "Show high priority notifications";
    high_priority.mode = FilterMatchMode::kInclude;
    high_priority.min_priority = NotificationPriority::kHigh;
    rules_.push_back(std::move(high_priority));

    // Default: show all normal notifications
    NotificationFilterRule show_normal;
    show_normal.rule_id = "default_show_normal";
    show_normal.description = "Show normal and above";
    show_normal.mode = FilterMatchMode::kInclude;
    show_normal.min_priority = NotificationPriority::kNormal;
    rules_.push_back(std::move(show_normal));
}

void NotificationFilter::clear_all()
{
    rules_.clear();
}

auto NotificationFilter::matches_rule(const CenterNotification& notification,
                                      const NotificationFilterRule& rule) const -> bool
{
    // Priority check
    if (notification.priority < rule.min_priority)
    {
        return false;
    }

    // Source pattern check
    if (!rule.source_pattern.empty() &&
        notification.source.find(rule.source_pattern) == std::string::npos)
    {
        return false;
    }

    // Group pattern check
    if (!rule.group_pattern.empty() &&
        notification.group.find(rule.group_pattern) == std::string::npos)
    {
        return false;
    }

    // Title pattern check
    if (!rule.title_pattern.empty() &&
        notification.title.find(rule.title_pattern) == std::string::npos)
    {
        return false;
    }

    return true;
}

} // namespace markamp::core
