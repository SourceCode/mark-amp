/// @file CalendarEngine.cpp
/// @brief V4 Phase 26 – Calendar Integration (pure-logic engine) implementation.

#include "ui/CalendarEngine.h"

#include "core/EventBus.h"
#include "core/Events.h"

namespace markamp::ui
{

// ============================================================================
// Constructor
// ============================================================================

CalendarEngine::CalendarEngine(markamp::core::EventBus& event_bus)
    : event_bus_(event_bus)
{
    // Default to the current month.
    auto now_tp = std::chrono::system_clock::now();
    auto now_days = std::chrono::floor<std::chrono::days>(now_tp);
    auto ymd = std::chrono::year_month_day{now_days};
    current_month_ = std::chrono::year_month{ymd.year(), ymd.month()};
    build_month_data();
}

// ============================================================================
// Today helper
// ============================================================================

auto CalendarEngine::today() const -> std::chrono::year_month_day
{
    auto now_tp = std::chrono::system_clock::now();
    auto now_days = std::chrono::floor<std::chrono::days>(now_tp);
    return std::chrono::year_month_day{now_days};
}

// ============================================================================
// Navigation
// ============================================================================

auto CalendarEngine::set_month(std::chrono::year_month month) -> void
{
    current_month_ = month;
    build_month_data();
    core::events::CalendarMonthChangedEvent set_event;
    set_event.year = static_cast<int>(current_month_.year());
    set_event.month = static_cast<unsigned>(current_month_.month());
    event_bus_.publish(set_event);
}

auto CalendarEngine::previous() -> void
{
    current_month_ -= std::chrono::months{1};
    build_month_data();
    core::events::CalendarMonthChangedEvent prev_event;
    prev_event.year = static_cast<int>(current_month_.year());
    prev_event.month = static_cast<unsigned>(current_month_.month());
    event_bus_.publish(prev_event);
}

auto CalendarEngine::next() -> void
{
    current_month_ += std::chrono::months{1};
    build_month_data();
    core::events::CalendarMonthChangedEvent next_event;
    next_event.year = static_cast<int>(current_month_.year());
    next_event.month = static_cast<unsigned>(current_month_.month());
    event_bus_.publish(next_event);
}

auto CalendarEngine::go_today() -> void
{
    auto today_ymd = today();
    set_month(std::chrono::year_month{today_ymd.year(), today_ymd.month()});
}

auto CalendarEngine::current_month() const -> std::chrono::year_month
{
    return current_month_;
}

// ============================================================================
// Selection
// ============================================================================

auto CalendarEngine::select_date(std::chrono::year_month_day date) -> void
{
    // Deselect previous.
    for (auto& day : days_)
    {
        day.is_selected = false;
    }

    selected_date_ = date;

    // Mark the new selection.
    for (auto& day : days_)
    {
        if (day.date == date)
        {
            day.is_selected = true;
        }
    }

    // Build date string YYYY-MM-DD.
    int year_val = static_cast<int>(date.year());
    unsigned month_val = static_cast<unsigned>(date.month());
    unsigned day_val = static_cast<unsigned>(date.day());

    std::string date_str = std::to_string(year_val) + "-";
    if (month_val < 10)
    {
        date_str += "0";
    }
    date_str += std::to_string(month_val) + "-";
    if (day_val < 10)
    {
        date_str += "0";
    }
    date_str += std::to_string(day_val);

    core::events::CalendarDateSelectedEvent select_event;
    select_event.date_string = date_str;
    event_bus_.publish(select_event);
}

auto CalendarEngine::selected_date() const -> std::optional<std::chrono::year_month_day>
{
    return selected_date_;
}

// ============================================================================
// Data setters
// ============================================================================

auto CalendarEngine::set_daily_notes(const std::set<std::chrono::year_month_day>& dates) -> void
{
    daily_note_dates_ = dates;
    // Update existing days.
    for (auto& day : days_)
    {
        day.has_daily_note = daily_note_dates_.count(day.date) > 0;
    }
}

auto CalendarEngine::set_tasks_due(
    const std::map<std::chrono::year_month_day, int>& date_task_counts) -> void
{
    task_due_counts_ = date_task_counts;
    for (auto& day : days_)
    {
        auto iter = task_due_counts_.find(day.date);
        day.task_count = (iter != task_due_counts_.end()) ? iter->second : 0;
    }
}

auto CalendarEngine::set_overdue(
    const std::map<std::chrono::year_month_day, int>& date_overdue_counts) -> void
{
    overdue_counts_ = date_overdue_counts;
    for (auto& day : days_)
    {
        auto iter = overdue_counts_.find(day.date);
        day.overdue_count = (iter != overdue_counts_.end()) ? iter->second : 0;
    }
}

// ============================================================================
// View mode
// ============================================================================

auto CalendarEngine::set_view(CalendarView view) -> void
{
    view_mode_ = view;
}

auto CalendarEngine::view_mode() const -> CalendarView
{
    return view_mode_;
}

// ============================================================================
// Get days
// ============================================================================

auto CalendarEngine::get_days() const -> const std::vector<CalendarDay>&
{
    return days_;
}

// ============================================================================
// Build month data
// ============================================================================

auto CalendarEngine::build_month_data() -> void
{
    days_.clear();

    auto today_ymd = today();

    // First day of the current month.
    auto first_day = current_month_ / std::chrono::day{1};

    // Last day of the current month.
    auto last_day = current_month_ / std::chrono::last;
    unsigned days_in_month = static_cast<unsigned>(last_day.day());

    // Find the weekday of the first day (0=Sun, 1=Mon, ..., 6=Sat).
    auto first_weekday = std::chrono::weekday{std::chrono::sys_days{first_day}};
    int padding_before = static_cast<int>(first_weekday.c_encoding()); // 0=Sun

    // Previous month days.
    auto prev_month = current_month_ - std::chrono::months{1};
    auto prev_last = prev_month / std::chrono::last;
    unsigned prev_days_in_month = static_cast<unsigned>(prev_last.day());

    // Fill padding from previous month.
    for (int idx = padding_before - 1; idx >= 0; --idx)
    {
        unsigned day_num = prev_days_in_month - static_cast<unsigned>(idx);
        auto date = std::chrono::year_month_day{
            prev_month.year(), prev_month.month(), std::chrono::day{day_num}};
        CalendarDay cal_day;
        cal_day.date = date;
        cal_day.is_current_month = false;
        cal_day.is_today = (date == today_ymd);
        cal_day.has_daily_note = daily_note_dates_.count(date) > 0;

        auto task_iter = task_due_counts_.find(date);
        cal_day.task_count = (task_iter != task_due_counts_.end()) ? task_iter->second : 0;

        auto overdue_iter = overdue_counts_.find(date);
        cal_day.overdue_count = (overdue_iter != overdue_counts_.end()) ? overdue_iter->second : 0;

        if (selected_date_.has_value() && *selected_date_ == date)
        {
            cal_day.is_selected = true;
        }

        days_.push_back(cal_day);
    }

    // Current month days.
    for (unsigned day_num = 1; day_num <= days_in_month; ++day_num)
    {
        auto date = std::chrono::year_month_day{
            current_month_.year(), current_month_.month(), std::chrono::day{day_num}};
        CalendarDay cal_day;
        cal_day.date = date;
        cal_day.is_current_month = true;
        cal_day.is_today = (date == today_ymd);
        cal_day.has_daily_note = daily_note_dates_.count(date) > 0;

        auto task_iter = task_due_counts_.find(date);
        cal_day.task_count = (task_iter != task_due_counts_.end()) ? task_iter->second : 0;

        auto overdue_iter = overdue_counts_.find(date);
        cal_day.overdue_count = (overdue_iter != overdue_counts_.end()) ? overdue_iter->second : 0;

        if (selected_date_.has_value() && *selected_date_ == date)
        {
            cal_day.is_selected = true;
        }

        days_.push_back(cal_day);
    }

    // Fill padding from next month to complete the grid (always 42 cells = 6 rows × 7 cols).
    auto next_month = current_month_ + std::chrono::months{1};
    unsigned day_num = 1;
    while (days_.size() < 42)
    {
        auto date = std::chrono::year_month_day{
            next_month.year(), next_month.month(), std::chrono::day{day_num}};
        CalendarDay cal_day;
        cal_day.date = date;
        cal_day.is_current_month = false;
        cal_day.is_today = (date == today_ymd);
        cal_day.has_daily_note = daily_note_dates_.count(date) > 0;

        auto task_iter = task_due_counts_.find(date);
        cal_day.task_count = (task_iter != task_due_counts_.end()) ? task_iter->second : 0;

        auto overdue_iter = overdue_counts_.find(date);
        cal_day.overdue_count = (overdue_iter != overdue_counts_.end()) ? overdue_iter->second : 0;

        if (selected_date_.has_value() && *selected_date_ == date)
        {
            cal_day.is_selected = true;
        }

        days_.push_back(cal_day);
        ++day_num;
    }
}

} // namespace markamp::ui
