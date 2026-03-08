// test_frame_scheduler.cpp — 10 tests for FrameScheduler
#include "core/FrameScheduler.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("FrameScheduler starts with no pending tasks", "[scheduler]")
{
    FrameScheduler scheduler;
    CHECK_FALSE(scheduler.has_pending());
    CHECK(scheduler.pending_count() == 0);
}

TEST_CASE("FrameScheduler enqueue increments pending count", "[scheduler]")
{
    FrameScheduler scheduler;
    scheduler.enqueue({TaskPriority::Background, {}, [] { return false; }});
    scheduler.enqueue({TaskPriority::Input, {}, [] { return false; }});
    CHECK(scheduler.pending_count() == 2);
}

TEST_CASE("FrameScheduler run_frame executes tasks", "[scheduler]")
{
    FrameScheduler scheduler;
    int executed = 0;
    scheduler.enqueue({TaskPriority::Input,
                       {},
                       [&executed]
                       {
                           ++executed;
                           return false;
                       }});
    scheduler.enqueue({TaskPriority::Paint,
                       {},
                       [&executed]
                       {
                           ++executed;
                           return false;
                       }});
    scheduler.run_frame(std::chrono::microseconds(1000000)); // generous 1s budget
    CHECK(executed == 2);
    CHECK_FALSE(scheduler.has_pending());
}

TEST_CASE("FrameScheduler executes higher priority first", "[scheduler]")
{
    FrameScheduler scheduler;
    std::vector<TaskPriority> order;
    scheduler.enqueue({TaskPriority::Background,
                       {},
                       [&order]
                       {
                           order.push_back(TaskPriority::Background);
                           return false;
                       }});
    scheduler.enqueue({TaskPriority::Input,
                       {},
                       [&order]
                       {
                           order.push_back(TaskPriority::Input);
                           return false;
                       }});
    scheduler.enqueue({TaskPriority::Paint,
                       {},
                       [&order]
                       {
                           order.push_back(TaskPriority::Paint);
                           return false;
                       }});
    scheduler.run_frame(std::chrono::microseconds(1000000));
    REQUIRE(order.size() == 3);
    CHECK(order[0] == TaskPriority::Input);
    CHECK(order[1] == TaskPriority::Paint);
    CHECK(order[2] == TaskPriority::Background);
}

TEST_CASE("FrameScheduler re-enqueues incomplete tasks", "[scheduler]")
{
    FrameScheduler scheduler;
    int call_count = 0;
    scheduler.enqueue({TaskPriority::Input,
                       {},
                       [&call_count]
                       {
                           ++call_count;
                           return call_count < 3; // needs 3 frames to complete
                       }});
    scheduler.run_frame(std::chrono::microseconds(1000000));
    CHECK(call_count == 1);
    CHECK(scheduler.has_pending());
    scheduler.run_frame(std::chrono::microseconds(1000000));
    CHECK(call_count == 2);
    scheduler.run_frame(std::chrono::microseconds(1000000));
    CHECK(call_count == 3);
    CHECK_FALSE(scheduler.has_pending());
}

TEST_CASE("FrameScheduler clear removes all tasks", "[scheduler]")
{
    FrameScheduler scheduler;
    scheduler.enqueue({TaskPriority::Input, {}, [] { return false; }});
    scheduler.enqueue({TaskPriority::Paint, {}, [] { return false; }});
    scheduler.clear();
    CHECK_FALSE(scheduler.has_pending());
    CHECK(scheduler.pending_count() == 0);
}

TEST_CASE("FrameScheduler zero budget executes nothing", "[scheduler]")
{
    FrameScheduler scheduler;
    int executed = 0;
    scheduler.enqueue({TaskPriority::Input,
                       {},
                       [&executed]
                       {
                           ++executed;
                           return false;
                       }});
    scheduler.run_frame(std::chrono::microseconds(0));
    CHECK(executed == 0);
    CHECK(scheduler.has_pending());
}

TEST_CASE("FrameScheduler all priority levels are ordered", "[scheduler]")
{
    CHECK(static_cast<uint8_t>(TaskPriority::Input) < static_cast<uint8_t>(TaskPriority::Paint));
    CHECK(static_cast<uint8_t>(TaskPriority::Paint) < static_cast<uint8_t>(TaskPriority::Layout));
    CHECK(static_cast<uint8_t>(TaskPriority::Layout) <
          static_cast<uint8_t>(TaskPriority::Highlight));
    CHECK(static_cast<uint8_t>(TaskPriority::Highlight) <
          static_cast<uint8_t>(TaskPriority::Search));
    CHECK(static_cast<uint8_t>(TaskPriority::Search) < static_cast<uint8_t>(TaskPriority::FileIO));
    CHECK(static_cast<uint8_t>(TaskPriority::FileIO) <
          static_cast<uint8_t>(TaskPriority::Background));
}

TEST_CASE("FrameScheduler handles empty run_frame", "[scheduler]")
{
    FrameScheduler scheduler;
    REQUIRE_NOTHROW(scheduler.run_frame(std::chrono::microseconds(1000000)));
}

TEST_CASE("FrameScheduler task completion removes from queue", "[scheduler]")
{
    FrameScheduler scheduler;
    scheduler.enqueue({TaskPriority::Input, {}, [] { return false; }});
    CHECK(scheduler.pending_count() == 1);
    scheduler.run_frame(std::chrono::microseconds(1000000));
    CHECK(scheduler.pending_count() == 0);
}
