/// @file NotificationFilter.h
/// @brief V9 Phase 39 — Notification filter rules for the notification center.
#pragma once

#include "NotificationCenter.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Match mode for filter rules.
enum class FilterMatchMode : uint8_t
{
    kInclude = 0, ///< Include matching notifications
    kExclude = 1, ///< Exclude matching notifications
};

/// A single notification filter rule.
/// Matches on source, group, priority level, or title substring.
struct NotificationFilterRule
{
    std::string rule_id;
    std::string description;
    FilterMatchMode mode{FilterMatchMode::kInclude};

    // Matching criteria (empty = match all)
    std::string source_pattern;                                    ///< Source must contain
    std::string group_pattern;                                     ///< Group must contain
    std::string title_pattern;                                     ///< Title must contain
    NotificationPriority min_priority{NotificationPriority::kLow}; ///< Minimum priority
    bool enabled{true};
};

/// Manages a set of notification filter rules.
/// Used by the NotificationCenter to decide which notifications to show.
class NotificationFilter
{
public:
    NotificationFilter() = default;

    // ── Rule management ───────────────────────────────────────────────
    void add_rule(NotificationFilterRule rule);
    auto remove_rule(const std::string& rule_id) -> bool;
    [[nodiscard]] auto find_rule(const std::string& rule_id) const -> const NotificationFilterRule*;
    [[nodiscard]] auto all_rules() const -> const std::vector<NotificationFilterRule>&;
    [[nodiscard]] auto rule_count() const -> int;

    // ── Rule state ────────────────────────────────────────────────────
    auto enable_rule(const std::string& rule_id) -> bool;
    auto disable_rule(const std::string& rule_id) -> bool;

    // ── Filtering ─────────────────────────────────────────────────────
    /// Check if a notification matches any include rules and no exclude rules.
    [[nodiscard]] auto should_show(const CenterNotification& notification) const -> bool;

    /// Apply filters to a list of notifications, returning those that pass.
    [[nodiscard]] auto
    apply_filters(const std::vector<const CenterNotification*>& notifications) const
        -> std::vector<const CenterNotification*>;

    // ── Built-in presets ──────────────────────────────────────────────
    void load_defaults();
    void clear_all();

private:
    std::vector<NotificationFilterRule> rules_;

    [[nodiscard]] auto matches_rule(const CenterNotification& notification,
                                    const NotificationFilterRule& rule) const -> bool;
};

} // namespace markamp::core
