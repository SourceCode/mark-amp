/// @file test_calendar.cpp
/// @brief V4 Phase 26 – Calendar Engine unit tests.

#include "core/EventBus.h"
#include "ui/CalendarEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

struct CalendarFixture
{
    EventBus event_bus;
    CalendarEngine engine{event_bus};
};

TEST_CASE("CalendarEngine: month data for Feb 2026", "[calendar]")
{
    CalendarFixture fixture;
    auto feb_2026 = std::chrono::year{2026} / std::chrono::February;
    fixture.engine.set_month(feb_2026);

    const auto& days = fixture.engine.get_days();
    REQUIRE(days.size() == 42); // Always 6x7 grid

    // Count current-month days (February 2026 has 28 days).
    int current_month_count = 0;
    for (const auto& day : days)
    {
        if (day.is_current_month)
        {
            ++current_month_count;
        }
    }
    REQUIRE(current_month_count == 28);
}

TEST_CASE("CalendarEngine: daily note indicators", "[calendar]")
{
    CalendarFixture fixture;
    auto jan_2026 = std::chrono::year{2026} / std::chrono::January;
    fixture.engine.set_month(jan_2026);

    // Set 3 daily notes.
    std::set<std::chrono::year_month_day> notes = {
        std::chrono::year{2026} / std::chrono::January / std::chrono::day{5},
        std::chrono::year{2026} / std::chrono::January / std::chrono::day{10},
        std::chrono::year{2026} / std::chrono::January / std::chrono::day{20}};
    fixture.engine.set_daily_notes(notes);

    const auto& days = fixture.engine.get_days();
    int note_count = 0;
    for (const auto& day : days)
    {
        if (day.has_daily_note)
        {
            ++note_count;
        }
    }
    REQUIRE(note_count == 3);
}

TEST_CASE("CalendarEngine: task count per day", "[calendar]")
{
    CalendarFixture fixture;
    auto jan_2026 = std::chrono::year{2026} / std::chrono::January;
    fixture.engine.set_month(jan_2026);

    auto jan_15 = std::chrono::year{2026} / std::chrono::January / std::chrono::day{15};
    std::map<std::chrono::year_month_day, int> tasks = {{jan_15, 2}};
    fixture.engine.set_tasks_due(tasks);

    const auto& days = fixture.engine.get_days();
    bool found = false;
    for (const auto& day : days)
    {
        if (day.date == jan_15)
        {
            REQUIRE(day.task_count == 2);
            found = true;
        }
    }
    REQUIRE(found);
}

TEST_CASE("CalendarEngine: today highlight", "[calendar]")
{
    CalendarFixture fixture;
    // Engine defaults to current month, so today should be marked.
    const auto& days = fixture.engine.get_days();
    int today_count = 0;
    for (const auto& day : days)
    {
        if (day.is_today)
        {
            ++today_count;
        }
    }
    REQUIRE(today_count == 1);
}

TEST_CASE("CalendarEngine: date selection", "[calendar]")
{
    CalendarFixture fixture;
    auto jan_2026 = std::chrono::year{2026} / std::chrono::January;
    fixture.engine.set_month(jan_2026);

    auto jan_10 = std::chrono::year{2026} / std::chrono::January / std::chrono::day{10};
    fixture.engine.select_date(jan_10);

    REQUIRE(fixture.engine.selected_date().has_value());
    REQUIRE(*fixture.engine.selected_date() == jan_10);

    // Verify the day is marked as selected in the grid.
    const auto& days = fixture.engine.get_days();
    bool found_selected = false;
    for (const auto& day : days)
    {
        if (day.date == jan_10)
        {
            REQUIRE(day.is_selected);
            found_selected = true;
        }
    }
    REQUIRE(found_selected);
}

TEST_CASE("CalendarEngine: month navigation forward", "[calendar]")
{
    CalendarFixture fixture;
    auto jan_2026 = std::chrono::year{2026} / std::chrono::January;
    fixture.engine.set_month(jan_2026);

    fixture.engine.next();
    auto expected = std::chrono::year{2026} / std::chrono::February;
    REQUIRE(fixture.engine.current_month() == expected);
}

TEST_CASE("CalendarEngine: month navigation backward", "[calendar]")
{
    CalendarFixture fixture;
    auto mar_2026 = std::chrono::year{2026} / std::chrono::March;
    fixture.engine.set_month(mar_2026);

    fixture.engine.previous();
    auto expected = std::chrono::year{2026} / std::chrono::February;
    REQUIRE(fixture.engine.current_month() == expected);
}

TEST_CASE("CalendarEngine: go today", "[calendar]")
{
    CalendarFixture fixture;
    // Navigate away.
    auto jan_2020 = std::chrono::year{2020} / std::chrono::January;
    fixture.engine.set_month(jan_2020);
    REQUIRE(fixture.engine.current_month() == jan_2020);

    // Go back to today.
    fixture.engine.go_today();

    // Should be current month.
    auto now_tp = std::chrono::system_clock::now();
    auto now_days = std::chrono::floor<std::chrono::days>(now_tp);
    auto ymd = std::chrono::year_month_day{now_days};
    auto expected = std::chrono::year_month{ymd.year(), ymd.month()};
    REQUIRE(fixture.engine.current_month() == expected);
}

TEST_CASE("CalendarEngine: overdue counts", "[calendar]")
{
    CalendarFixture fixture;
    auto jan_2026 = std::chrono::year{2026} / std::chrono::January;
    fixture.engine.set_month(jan_2026);

    auto jan_5 = std::chrono::year{2026} / std::chrono::January / std::chrono::day{5};
    std::map<std::chrono::year_month_day, int> overdue = {{jan_5, 3}};
    fixture.engine.set_overdue(overdue);

    const auto& days = fixture.engine.get_days();
    for (const auto& day : days)
    {
        if (day.date == jan_5)
        {
            REQUIRE(day.overdue_count == 3);
        }
    }
}

TEST_CASE("CalendarEngine: non-current-month padding", "[calendar]")
{
    CalendarFixture fixture;
    auto feb_2026 = std::chrono::year{2026} / std::chrono::February;
    fixture.engine.set_month(feb_2026);

    const auto& days = fixture.engine.get_days();
    // There should be padding days that are NOT current month.
    int non_current = 0;
    for (const auto& day : days)
    {
        if (!day.is_current_month)
        {
            ++non_current;
        }
    }
    REQUIRE(non_current > 0);
    REQUIRE(non_current == 42 - 28); // Feb 2026 has 28 days, grid is 42.
}
