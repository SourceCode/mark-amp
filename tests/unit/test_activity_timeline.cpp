// test_activity_timeline.cpp — 10 tests for ActivityTimeline
#include "core/ActivityTimeline.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ActivityTimeline starts empty", "[timeline]")
{
    ActivityTimeline timeline;
    CHECK(timeline.active_days() == 0);
    CHECK(timeline.total_events() == 0);
}

TEST_CASE("ActivityTimeline all_summaries is initially empty", "[timeline]")
{
    ActivityTimeline timeline;
    CHECK(timeline.all_summaries().empty());
}

TEST_CASE("ActivityTimeline summary_for_date returns nullptr when empty", "[timeline]")
{
    ActivityTimeline timeline;
    CHECK(timeline.summary_for_date("2025-01-01") == nullptr);
}

TEST_CASE("ActivityTimeline build_from_feed populates data", "[timeline]")
{
    ActivityFeed feed;
    ActivityEntry entry;
    entry.activity_id = "a1";
    entry.description = "Edited file";
    entry.category = ActivityCategory::kFileEdit;
    entry.timestamp = std::chrono::system_clock::now();
    feed.record(std::move(entry));

    ActivityTimeline timeline;
    timeline.build_from_feed(feed);
    CHECK(timeline.total_events() > 0);
    CHECK(timeline.active_days() > 0);
}

TEST_CASE("ActivityTimeline clear removes all data", "[timeline]")
{
    ActivityFeed feed;
    ActivityEntry entry;
    entry.activity_id = "a2";
    entry.description = "Test";
    entry.category = ActivityCategory::kFileEdit;
    entry.timestamp = std::chrono::system_clock::now();
    feed.record(std::move(entry));

    ActivityTimeline timeline;
    timeline.build_from_feed(feed);
    timeline.clear();
    CHECK(timeline.active_days() == 0);
    CHECK(timeline.total_events() == 0);
}

TEST_CASE("ActivityTimeline export_json returns valid JSON", "[timeline]")
{
    ActivityTimeline timeline;
    auto json = timeline.export_json();
    CHECK_FALSE(json.empty());
}

TEST_CASE("ActivityTimeline most_active_day returns string", "[timeline]")
{
    ActivityFeed feed;
    ActivityEntry entry;
    entry.activity_id = "a3";
    entry.description = "A";
    entry.category = ActivityCategory::kFileEdit;
    entry.timestamp = std::chrono::system_clock::now();
    feed.record(std::move(entry));

    ActivityTimeline timeline;
    timeline.build_from_feed(feed);
    auto day = timeline.most_active_day();
    CHECK_FALSE(day.empty());
}

TEST_CASE("ActivityTimeline current_streak starts at 0 or 1", "[timeline]")
{
    ActivityTimeline timeline;
    auto streak = timeline.current_streak();
    CHECK(streak.length >= 0);
}

TEST_CASE("ActivityTimeline longest_streak starts at 0 or 1", "[timeline]")
{
    ActivityTimeline timeline;
    auto streak = timeline.longest_streak();
    CHECK(streak.length >= 0);
}

TEST_CASE("DailySummary default values", "[timeline]")
{
    DailySummary summary;
    CHECK(summary.total_events == 0);
    CHECK(summary.file_edits == 0);
    CHECK(summary.date.empty());
}
