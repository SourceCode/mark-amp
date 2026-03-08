// test_change_tracker.cpp — 10 tests for ChangeTracker
#include "core/ChangeTracker.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ChangeTracker starts with no changes", "[change_tracker]")
{
    ChangeTracker tracker;
    CHECK_FALSE(tracker.has_changes());
    CHECK(tracker.change_count() == 0);
}

TEST_CASE("ChangeTracker record_edit marks line", "[change_tracker]")
{
    ChangeTracker tracker;
    tracker.record_edit(5);
    CHECK(tracker.is_changed(5));
    CHECK(tracker.change_count() == 1);
}

TEST_CASE("ChangeTracker record_edit_range marks range", "[change_tracker]")
{
    ChangeTracker tracker;
    tracker.record_edit_range(3, 7);
    CHECK(tracker.is_changed(3));
    CHECK(tracker.is_changed(5));
    CHECK(tracker.is_changed(7));
    CHECK_FALSE(tracker.is_changed(2));
    CHECK(tracker.change_count() == 5); // lines 3,4,5,6,7
}

TEST_CASE("ChangeTracker mark_saved clears all changes", "[change_tracker]")
{
    ChangeTracker tracker;
    tracker.record_edit(1);
    tracker.record_edit(2);
    tracker.mark_saved();
    CHECK_FALSE(tracker.has_changes());
    CHECK(tracker.change_count() == 0);
}

TEST_CASE("ChangeTracker next_change finds forward", "[change_tracker]")
{
    ChangeTracker tracker;
    tracker.record_edit(5);
    tracker.record_edit(10);
    tracker.record_edit(15);
    CHECK(tracker.next_change(3) == 5);
    CHECK(tracker.next_change(7) == 10);
}

TEST_CASE("ChangeTracker prev_change finds backward", "[change_tracker]")
{
    ChangeTracker tracker;
    tracker.record_edit(5);
    tracker.record_edit(10);
    tracker.record_edit(15);
    CHECK(tracker.prev_change(12) == 10);
    CHECK(tracker.prev_change(20) == 15);
}

TEST_CASE("ChangeTracker next_change wraps around", "[change_tracker]")
{
    ChangeTracker tracker;
    tracker.record_edit(5);
    tracker.record_edit(10);
    // Past all changes should wrap to first
    int next = tracker.next_change(100);
    CHECK(next == 5);
}

TEST_CASE("ChangeTracker changed_lines returns sorted", "[change_tracker]")
{
    ChangeTracker tracker;
    tracker.record_edit(10);
    tracker.record_edit(3);
    tracker.record_edit(7);
    auto lines = tracker.changed_lines();
    REQUIRE(lines.size() == 3);
    CHECK(lines[0] == 3);
    CHECK(lines[1] == 7);
    CHECK(lines[2] == 10);
}

TEST_CASE("ChangeTracker duplicate edits do not double count", "[change_tracker]")
{
    ChangeTracker tracker;
    tracker.record_edit(5);
    tracker.record_edit(5);
    tracker.record_edit(5);
    CHECK(tracker.change_count() == 1);
}

TEST_CASE("ChangeTracker next_change returns -1 when empty", "[change_tracker]")
{
    ChangeTracker tracker;
    CHECK(tracker.next_change(0) == -1);
    CHECK(tracker.prev_change(0) == -1);
}
