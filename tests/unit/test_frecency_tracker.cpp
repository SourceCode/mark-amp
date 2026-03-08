// test_frecency_tracker.cpp — 10 tests for FrecencyTracker
#include "core/FrecencyTracker.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("FrecencyTracker starts with zero entries", "[frecency]")
{
    FrecencyTracker tracker;
    CHECK(tracker.entry_count() == 0);
    CHECK_FALSE(tracker.has_key("anything"));
}

TEST_CASE("FrecencyTracker record_access adds entry", "[frecency]")
{
    FrecencyTracker tracker;
    tracker.record_access("file.md", 1000);
    CHECK(tracker.has_key("file.md"));
    CHECK(tracker.entry_count() == 1);
}

TEST_CASE("FrecencyTracker multiple accesses increase access count", "[frecency]")
{
    FrecencyTracker tracker;
    tracker.record_access("file.md", 1000);
    tracker.record_access("file.md", 2000);
    tracker.record_access("file.md", 3000);
    const auto* entry = tracker.get_entry("file.md");
    REQUIRE(entry != nullptr);
    CHECK(entry->access_count == 3);
}

TEST_CASE("FrecencyTracker score is higher for recent accesses", "[frecency]")
{
    FrecencyTracker tracker;
    const int64_t now = 1000000;
    // "recent" was accessed 1 second ago
    tracker.record_access("recent", now - 1);
    // "old" was accessed 31 days ago
    tracker.record_access("old", now - 31 * 24 * 3600);

    double recent_score = tracker.score("recent", now);
    double old_score = tracker.score("old", now);
    CHECK(recent_score > old_score);
}

TEST_CASE("FrecencyTracker ranked_keys returns top N by score", "[frecency]")
{
    FrecencyTracker tracker;
    const int64_t now = 100000;
    tracker.record_access("a", now - 1);
    tracker.record_access("b", now - 86400); // 1 day ago
    tracker.record_access("c", now - 1);
    tracker.record_access("c", now - 1); // 2 accesses

    auto ranked = tracker.ranked_keys(2, now);
    CHECK(ranked.size() == 2);
    // "c" has more accesses + recency, should rank first
    CHECK(ranked[0] == "c");
}

TEST_CASE("FrecencyTracker ranked_keys limits output", "[frecency]")
{
    FrecencyTracker tracker;
    const int64_t now = 100000;
    tracker.record_access("a", now);
    tracker.record_access("b", now);
    tracker.record_access("c", now);
    tracker.record_access("d", now);

    auto ranked = tracker.ranked_keys(2, now);
    CHECK(ranked.size() == 2);
}

TEST_CASE("FrecencyTracker clear removes all entries", "[frecency]")
{
    FrecencyTracker tracker;
    tracker.record_access("x", 1000);
    tracker.record_access("y", 2000);
    tracker.clear();
    CHECK(tracker.entry_count() == 0);
    CHECK_FALSE(tracker.has_key("x"));
}

TEST_CASE("FrecencyTracker score returns 0 for unknown key", "[frecency]")
{
    FrecencyTracker tracker;
    CHECK(tracker.score("nonexistent") == 0.0);
}

TEST_CASE("FrecencyTracker decay weights are correctly ordered", "[frecency]")
{
    CHECK(FrecencyTracker::kWeight4Hours > FrecencyTracker::kWeight1Day);
    CHECK(FrecencyTracker::kWeight1Day > FrecencyTracker::kWeight3Days);
    CHECK(FrecencyTracker::kWeight3Days > FrecencyTracker::kWeight7Days);
    CHECK(FrecencyTracker::kWeight7Days > FrecencyTracker::kWeight30Days);
    CHECK(FrecencyTracker::kWeight30Days > FrecencyTracker::kWeightOlder);
}

TEST_CASE("FrecencyTracker timestamps capped at kMaxTimestamps", "[frecency]")
{
    FrecencyTracker tracker;
    for (int i = 0; i < 20; ++i)
    {
        tracker.record_access("key", 1000 + i);
    }
    const auto* entry = tracker.get_entry("key");
    REQUIRE(entry != nullptr);
    CHECK(entry->access_timestamps.size() <= static_cast<size_t>(FrecencyEntry::kMaxTimestamps));
}
