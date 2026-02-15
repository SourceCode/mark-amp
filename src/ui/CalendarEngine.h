/// @file CalendarEngine.h
/// @brief V4 Phase 26 – Calendar Integration (pure-logic engine).
/// Builds month/week calendar data, tracks daily notes and task due dates,
/// handles navigation. No wxWidgets dependency — UI rendering is separate.

#pragma once

#include <chrono>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;
}

namespace markamp::ui
{

/// Represents a single day in the calendar grid.
struct CalendarDay
{
    std::chrono::year_month_day date;
    bool has_daily_note{false};
    int task_count{0};
    int overdue_count{0};
    bool is_today{false};
    bool is_selected{false};
    bool is_current_month{false};
};

/// Calendar view modes.
enum class CalendarView : uint8_t
{
    kMonth,
    kWeek,
    kDay
};

/// Pure-logic calendar engine.
/// Computes the grid of CalendarDay entries for a given month, integrates
/// daily-note and task-due-date information, and handles navigation.
class CalendarEngine
{
public:
    explicit CalendarEngine(markamp::core::EventBus& event_bus);

    /// Set the displayed month and rebuild data.
    auto set_month(std::chrono::year_month month) -> void;

    /// Navigate to the previous month.
    auto previous() -> void;

    /// Navigate to the next month.
    auto next() -> void;

    /// Navigate to today's month.
    auto go_today() -> void;

    /// Get the current displayed month.
    [[nodiscard]] auto current_month() const -> std::chrono::year_month;

    /// Select a specific date.
    auto select_date(std::chrono::year_month_day date) -> void;

    /// Get the currently selected date (if any).
    [[nodiscard]] auto selected_date() const -> std::optional<std::chrono::year_month_day>;

    /// Set dates that have daily notes.
    auto set_daily_notes(const std::set<std::chrono::year_month_day>& dates) -> void;

    /// Set task due-date counts (date → number of tasks due on that date).
    auto set_tasks_due(const std::map<std::chrono::year_month_day, int>& date_task_counts) -> void;

    /// Set overdue task counts.
    auto set_overdue(const std::map<std::chrono::year_month_day, int>& date_overdue_counts) -> void;

    /// Get the fully-built grid of CalendarDay entries (typically 35 or 42).
    [[nodiscard]] auto get_days() const -> const std::vector<CalendarDay>&;

    /// Rebuild the month data from scratch.
    auto build_month_data() -> void;

    /// Set the view mode.
    auto set_view(CalendarView view) -> void;

    /// Get the current view mode.
    [[nodiscard]] auto view_mode() const -> CalendarView;

private:
    markamp::core::EventBus& event_bus_;

    std::chrono::year_month current_month_;
    CalendarView view_mode_{CalendarView::kMonth};
    std::vector<CalendarDay> days_;
    std::optional<std::chrono::year_month_day> selected_date_;

    std::set<std::chrono::year_month_day> daily_note_dates_;
    std::map<std::chrono::year_month_day, int> task_due_counts_;
    std::map<std::chrono::year_month_day, int> overdue_counts_;

    [[nodiscard]] auto today() const -> std::chrono::year_month_day;
};

} // namespace markamp::ui
