/// @file TaskRecurrence.h
/// @brief V9 Phase 23 – Recurring task pattern engine.
/// Parses recurrence syntax from task text (@daily, @every(2w), @monthly(15)),
/// computes next occurrences, and generates date sequences.

#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Recurrence enums and types
// ============================================================================

enum class RecurrencePattern : uint8_t
{
    kDaily,
    kWeekly,
    kMonthly,
    kYearly,
    kCustom
};

/// Which days of the week a weekly recurrence fires on.
enum class Weekday : uint8_t
{
    kMonday,
    kTuesday,
    kWednesday,
    kThursday,
    kFriday,
    kSaturday,
    kSunday
};

/// A parsed recurrence rule.
struct RecurrenceRule
{
    RecurrencePattern pattern{RecurrencePattern::kDaily};
    int interval{1};                                     ///< Every N periods (e.g. every 2 weeks)
    std::vector<Weekday> days_of_week;                   ///< For weekly patterns
    int day_of_month{1};                                 ///< For monthly patterns
    std::optional<std::chrono::year_month_day> end_date; ///< Optional end date
    int max_occurrences{0};                              ///< 0 = unlimited
};

// ============================================================================
// TaskRecurrence
// ============================================================================

class TaskRecurrence
{
public:
    TaskRecurrence() = default;

    /// Parse recurrence syntax from task text.
    /// Supports: @daily, @weekly, @monthly, @monthly(15), @yearly,
    ///           @every(2d), @every(3w), @every(2m), @every(1y)
    [[nodiscard]] static auto parse_recurrence(const std::string& text)
        -> std::optional<RecurrenceRule>;

    /// Quick check for recurrence syntax.
    [[nodiscard]] static auto is_recurring(const std::string& text) -> bool;

    /// Compute the next occurrence after a given date.
    [[nodiscard]] static auto next_occurrence(const RecurrenceRule& rule,
                                              std::chrono::year_month_day from_date)
        -> std::chrono::year_month_day;

    /// Generate all occurrences within a date range.
    [[nodiscard]] static auto generate_occurrences(const RecurrenceRule& rule,
                                                   std::chrono::year_month_day start,
                                                   std::chrono::year_month_day end)
        -> std::vector<std::chrono::year_month_day>;

    /// Format a recurrence rule as a human-readable label.
    [[nodiscard]] static auto format_recurrence(const RecurrenceRule& rule) -> std::string;

private:
    /// Parse @every(Nd/Nw/Nm/Ny) patterns.
    [[nodiscard]] static auto parse_every_pattern(const std::string& text)
        -> std::optional<RecurrenceRule>;

    /// Advance a date by rule interval.
    [[nodiscard]] static auto advance_date(std::chrono::year_month_day date,
                                           const RecurrenceRule& rule)
        -> std::chrono::year_month_day;
};

} // namespace markamp::core
