# Phase 26 -- Calendar Integration and Daily Notes Calendar

## Objective

Implement a calendar panel that visualizes daily notes, tasks with due dates, and scheduled items on a month/week/day calendar view. Clicking a date opens or creates the daily note. Tasks appear as dots/markers on their due dates. Integrates with DailyNoteService (Phase 09) and TaskService (Phase 24).

## Prerequisites

- Phase 09 (DailyNoteService -- daily note listing and creation)
- Phase 24 (TaskService -- tasks with due dates)

## Feature References (PRD)

- PRD #30: Calendar Integration

## Data Structures to Implement

### File: `src/ui/CalendarPanel.h`

```cpp
#pragma once

#include <chrono>
#include <vector>
#include <wx/panel.h>

namespace markamp::core
{
class EventBus;
class ThemeEngine;
class DailyNoteService;
class TaskService;
}

namespace markamp::ui
{

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

enum class CalendarView : uint8_t { Month, Week, Day };

class CalendarPanel : public wxPanel
{
public:
    CalendarPanel(wxWindow* parent,
                  markamp::core::EventBus& event_bus,
                  markamp::core::DailyNoteService& daily_note_service,
                  markamp::core::TaskService& task_service,
                  markamp::core::ThemeEngine& theme_engine);

    /// Set the displayed month.
    auto set_month(std::chrono::year_month month) -> void;

    /// Navigate to previous month/week.
    auto previous() -> void;

    /// Navigate to next month/week.
    auto next() -> void;

    /// Go to today.
    auto go_today() -> void;

    /// Set the calendar view mode.
    auto set_view(CalendarView view) -> void;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::DailyNoteService& daily_note_service_;
    markamp::core::TaskService& task_service_;
    markamp::core::ThemeEngine& theme_engine_;

    std::chrono::year_month current_month_;
    CalendarView view_mode_{CalendarView::Month};
    std::vector<CalendarDay> days_;
    std::chrono::year_month_day selected_date_;

    auto build_month_data() -> void;
    auto on_paint(wxPaintEvent& event) -> void;
    auto on_click(wxMouseEvent& event) -> void;
    auto on_double_click(wxMouseEvent& event) -> void;

    auto draw_month_grid(wxDC& dc) -> void;
    auto draw_day_cell(wxDC& dc, const CalendarDay& day, int x, int y, int w, int h) -> void;

    [[nodiscard]] auto hit_test_day(int screen_x, int screen_y) const
        -> std::optional<std::chrono::year_month_day>;

    auto apply_theme() -> void;

    markamp::core::Subscription task_changed_sub_;
    markamp::core::Subscription daily_note_sub_;
    markamp::core::Subscription theme_changed_sub_;
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`build_month_data()`** -- For each day in the displayed month: check DailyNoteService for existing daily note. Count tasks due on that date from TaskService. Build CalendarDay entries.

2. **`draw_month_grid(dc)`** -- Draw 7-column grid (Sun-Sat). Draw day headers. For each day, call draw_day_cell.

3. **`draw_day_cell(dc, day, x, y, w, h)`** -- Draw day number. If has_daily_note, draw a dot indicator. If task_count > 0, draw task count badge. Highlight today. Highlight selected date. Dim dates outside current month.

4. **`on_click(event)`** -- Hit test to find clicked date. Select it. Show daily note content in a tooltip or info panel.

5. **`on_double_click(event)`** -- Open or create daily note for the clicked date via DailyNoteService.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CalendarDateSelectedEvent)
std::string date_string;  // YYYY-MM-DD
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CalendarMonthChangedEvent)
int year{0};
int month{0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_calendar_panel.cpp`

1. **Month data building** -- February 2026. Verify 28 CalendarDay entries for the month.
2. **Daily note indicator** -- 3 daily notes in Jan. Verify has_daily_note=true for those dates.
3. **Task count** -- 2 tasks due Jan 15. Verify task_count=2 for that day.
4. **Today highlight** -- Today's date has is_today=true.
5. **Click selects date** -- Click on Jan 10. Verify CalendarDateSelectedEvent with "2026-01-10".
6. **Double-click opens daily note** -- Double-click date. Verify DailyNoteOpenedEvent.
7. **Month navigation** -- Navigate next. Verify month increments.
8. **Go today** -- Navigate away. go_today() returns to current month.
9. **Overdue tasks** -- Past date with incomplete task. Verify overdue_count > 0.
10. **Outside month dates dimmed** -- Jan 31 showing in Feb view. Verify is_current_month=false.

## Acceptance Criteria

- [ ] Calendar displays month grid with day numbers
- [ ] Daily note indicators show which dates have notes
- [ ] Task due date counts displayed on calendar days
- [ ] Click selects date, double-click opens daily note
- [ ] Month navigation (previous/next) works
- [ ] Today is highlighted, non-current-month dates are dimmed
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/ui/CalendarPanel.h` | CalendarPanel, CalendarDay |
| CREATE | `src/ui/CalendarPanel.cpp` | Full implementation |
| MODIFY | `src/ui/MainFrame.cpp` | Register CalendarPanel in sidebar |
| MODIFY | `src/core/Events.h` | Add 2 calendar events |
| MODIFY | `src/CMakeLists.txt` | Add CalendarPanel.cpp |
| CREATE | `tests/unit/test_calendar_panel.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_calendar_panel target |

## Estimated Complexity

**M** -- Calendar grid drawing, date arithmetic, integration with two services, 10 tests.
