/// @file TaskRecurrence.cpp
/// @brief V9 Phase 23 – Recurring task pattern engine implementation.

#include "core/TaskRecurrence.h"

#include <algorithm>
#include <regex>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Public API
// ============================================================================

auto TaskRecurrence::parse_recurrence(const std::string& text) -> std::optional<RecurrenceRule>
{
    // Try simple keywords first
    static const std::regex kDailyRegex(R"(@daily\b)", std::regex::icase);
    static const std::regex kWeeklyRegex(R"(@weekly\b)", std::regex::icase);
    static const std::regex kMonthlyRegex(R"(@monthly(?:\((\d+)\))?)", std::regex::icase);
    static const std::regex kYearlyRegex(R"(@yearly\b)", std::regex::icase);

    std::smatch match;

    if (std::regex_search(text, match, kDailyRegex))
    {
        RecurrenceRule rule;
        rule.pattern = RecurrencePattern::kDaily;
        rule.interval = 1;
        return rule;
    }

    if (std::regex_search(text, match, kWeeklyRegex))
    {
        RecurrenceRule rule;
        rule.pattern = RecurrencePattern::kWeekly;
        rule.interval = 1;
        return rule;
    }

    if (std::regex_search(text, match, kMonthlyRegex))
    {
        RecurrenceRule rule;
        rule.pattern = RecurrencePattern::kMonthly;
        rule.interval = 1;
        if (match[1].matched)
        {
            rule.day_of_month = std::stoi(match[1].str());
        }
        return rule;
    }

    if (std::regex_search(text, match, kYearlyRegex))
    {
        RecurrenceRule rule;
        rule.pattern = RecurrencePattern::kYearly;
        rule.interval = 1;
        return rule;
    }

    // Try @every(Nd/Nw/Nm/Ny) pattern
    return parse_every_pattern(text);
}

auto TaskRecurrence::is_recurring(const std::string& text) -> bool
{
    static const std::regex kRecurrenceRegex(
        R"(@(?:daily|weekly|monthly|yearly|every\(\d+[dwmy]\)))", std::regex::icase);
    return std::regex_search(text, kRecurrenceRegex);
}

auto TaskRecurrence::next_occurrence(const RecurrenceRule& rule,
                                     std::chrono::year_month_day from_date)
    -> std::chrono::year_month_day
{
    auto next = advance_date(from_date, rule);

    // Respect end_date constraint
    if (rule.end_date.has_value())
    {
        auto end_sys = std::chrono::sys_days{rule.end_date.value()};
        auto next_sys = std::chrono::sys_days{next};
        if (next_sys > end_sys)
        {
            return from_date; // No more occurrences
        }
    }

    return next;
}

auto TaskRecurrence::generate_occurrences(const RecurrenceRule& rule,
                                          std::chrono::year_month_day start,
                                          std::chrono::year_month_day end)
    -> std::vector<std::chrono::year_month_day>
{
    std::vector<std::chrono::year_month_day> result;
    auto current = start;
    auto end_sys = std::chrono::sys_days{end};
    int count = 0;
    constexpr int kMaxOccurrences = 1000; // Safety limit

    while (std::chrono::sys_days{current} <= end_sys && count < kMaxOccurrences)
    {
        result.push_back(current);
        ++count;

        if (rule.max_occurrences > 0 && count >= rule.max_occurrences)
        {
            break;
        }

        auto next = advance_date(current, rule);
        if (next == current)
        {
            break; // Prevent infinite loop
        }
        current = next;
    }

    return result;
}

auto TaskRecurrence::format_recurrence(const RecurrenceRule& rule) -> std::string
{
    std::ostringstream oss;

    switch (rule.pattern)
    {
        case RecurrencePattern::kDaily:
            if (rule.interval == 1)
            {
                oss << "Daily";
            }
            else
            {
                oss << "Every " << rule.interval << " days";
            }
            break;

        case RecurrencePattern::kWeekly:
            if (rule.interval == 1)
            {
                oss << "Weekly";
            }
            else
            {
                oss << "Every " << rule.interval << " weeks";
            }
            break;

        case RecurrencePattern::kMonthly:
            if (rule.interval == 1)
            {
                oss << "Monthly";
                if (rule.day_of_month > 0)
                {
                    oss << " on day " << rule.day_of_month;
                }
            }
            else
            {
                oss << "Every " << rule.interval << " months";
            }
            break;

        case RecurrencePattern::kYearly:
            if (rule.interval == 1)
            {
                oss << "Yearly";
            }
            else
            {
                oss << "Every " << rule.interval << " years";
            }
            break;

        case RecurrencePattern::kCustom:
            oss << "Custom (every " << rule.interval << " days)";
            break;
    }

    return oss.str();
}

// ============================================================================
// Private helpers
// ============================================================================

auto TaskRecurrence::parse_every_pattern(const std::string& text) -> std::optional<RecurrenceRule>
{
    static const std::regex kEveryRegex(R"(@every\((\d+)([dwmy])\))", std::regex::icase);

    std::smatch match;
    if (!std::regex_search(text, match, kEveryRegex))
    {
        return std::nullopt;
    }

    RecurrenceRule rule;
    rule.interval = std::stoi(match[1].str());

    const char unit = static_cast<char>(std::tolower(match[2].str()[0]));
    switch (unit)
    {
        case 'd':
            rule.pattern = RecurrencePattern::kDaily;
            break;
        case 'w':
            rule.pattern = RecurrencePattern::kWeekly;
            break;
        case 'm':
            rule.pattern = RecurrencePattern::kMonthly;
            break;
        case 'y':
            rule.pattern = RecurrencePattern::kYearly;
            break;
        default:
            return std::nullopt;
    }

    return rule;
}

auto TaskRecurrence::advance_date(std::chrono::year_month_day date, const RecurrenceRule& rule)
    -> std::chrono::year_month_day
{
    auto sys_date = std::chrono::sys_days{date};

    switch (rule.pattern)
    {
        case RecurrencePattern::kDaily:
        case RecurrencePattern::kCustom:
        {
            sys_date += std::chrono::days{rule.interval};
            return std::chrono::year_month_day{sys_date};
        }

        case RecurrencePattern::kWeekly:
        {
            sys_date += std::chrono::days{7 * rule.interval};
            return std::chrono::year_month_day{sys_date};
        }

        case RecurrencePattern::kMonthly:
        {
            auto year_month = date.year() / date.month();
            year_month += std::chrono::months{rule.interval};

            // Clamp day to last valid day of the target month
            auto target_day = std::chrono::day{static_cast<unsigned>(
                rule.day_of_month > 0 ? rule.day_of_month
                                      : static_cast<int>(static_cast<unsigned>(date.day())))};
            auto result = year_month / target_day;
            if (!result.ok())
            {
                result = year_month / std::chrono::last;
            }
            return result;
        }

        case RecurrencePattern::kYearly:
        {
            auto target_year = date.year() + std::chrono::years{rule.interval};
            auto result = target_year / date.month() / date.day();
            if (!result.ok())
            {
                result = target_year / date.month() / std::chrono::last;
            }
            return result;
        }
    }

    return date; // Unreachable
}

} // namespace markamp::core
