// test_coalescing_task.cpp — 10 tests for CancelToken and CoalescingTask
#include "core/CoalescingTask.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// CancelToken tests
// ============================================================================

TEST_CASE("CancelToken starts not cancelled", "[coalescing][cancel_token]")
{
    CancelToken token;
    CHECK_FALSE(token.stop_requested());
}

TEST_CASE("CancelToken request_stop sets cancelled", "[coalescing][cancel_token]")
{
    CancelToken token;
    token.request_stop();
    CHECK(token.stop_requested());
}

TEST_CASE("CancelToken copies share state", "[coalescing][cancel_token]")
{
    CancelToken token;
    CancelToken copy = token;
    token.request_stop();
    CHECK(copy.stop_requested());
}

// ============================================================================
// CoalescingTask tests
// ============================================================================

TEST_CASE("CoalescingTask starts at version 0", "[coalescing]")
{
    CoalescingTask task;
    CHECK(task.current_version() == 0);
    CHECK_FALSE(task.stop_requested());
}

TEST_CASE("CoalescingTask submit returns fresh token", "[coalescing]")
{
    CoalescingTask task;
    auto token = task.submit(1);
    CHECK_FALSE(token.stop_requested());
    CHECK(task.current_version() == 1);
}

TEST_CASE("CoalescingTask submit cancels previous token", "[coalescing]")
{
    CoalescingTask task;
    auto token1 = task.submit(1);
    auto token2 = task.submit(2);
    CHECK(token1.stop_requested());       // old token cancelled
    CHECK_FALSE(token2.stop_requested()); // new token ok
}

TEST_CASE("CoalescingTask is_current checks version", "[coalescing]")
{
    CoalescingTask task;
    task.submit(5);
    CHECK(task.is_current(5));
    CHECK_FALSE(task.is_current(4));
    task.submit(6);
    CHECK_FALSE(task.is_current(5));
    CHECK(task.is_current(6));
}

TEST_CASE("CoalescingTask cancel stops current token", "[coalescing]")
{
    CoalescingTask task;
    auto token = task.submit(1);
    CHECK_FALSE(token.stop_requested());
    task.cancel();
    CHECK(token.stop_requested());
    CHECK(task.stop_requested());
}

TEST_CASE("CoalescingTask rapid submit chain cancels all previous", "[coalescing]")
{
    CoalescingTask task;
    auto t1 = task.submit(1);
    auto t2 = task.submit(2);
    auto t3 = task.submit(3);
    auto t4 = task.submit(4);
    CHECK(t1.stop_requested());
    CHECK(t2.stop_requested());
    CHECK(t3.stop_requested());
    CHECK_FALSE(t4.stop_requested());
    CHECK(task.current_version() == 4);
}

TEST_CASE("CoalescingTask version monotonically increases on submit", "[coalescing]")
{
    CoalescingTask task;
    task.submit(10);
    CHECK(task.current_version() == 10);
    task.submit(20);
    CHECK(task.current_version() == 20);
}
