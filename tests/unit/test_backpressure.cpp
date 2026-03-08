// test_backpressure.cpp — 10 tests for BackpressureQueue and RateLimiter
#include "core/Backpressure.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// BackpressureQueue tests
// ============================================================================

TEST_CASE("BackpressureQueue starts empty", "[backpressure][queue]")
{
    BackpressureQueue<int> q(10);
    CHECK(q.empty());
    CHECK(q.size() == 0);
    CHECK(q.capacity() == 10);
}

TEST_CASE("BackpressureQueue push and pop", "[backpressure][queue]")
{
    BackpressureQueue<int> q(10);
    auto result = q.push(42);
    CHECK(result.has_value());
    CHECK(q.size() == 1);
    auto val = q.try_pop();
    REQUIRE(val.has_value());
    CHECK(*val == 42);
    CHECK(q.empty());
}

TEST_CASE("BackpressureQueue DropOldest policy", "[backpressure][queue]")
{
    BackpressureQueue<int> q(3, OverflowPolicy::DropOldest);
    q.push(1);
    q.push(2);
    q.push(3);
    q.push(4); // drops 1
    CHECK(q.size() == 3);
    CHECK(q.dropped_count() == 1);
    auto val = q.try_pop();
    CHECK(*val == 2); // 1 was dropped
}

TEST_CASE("BackpressureQueue Reject policy", "[backpressure][queue]")
{
    BackpressureQueue<int> q(2, OverflowPolicy::Reject);
    q.push(1);
    q.push(2);
    auto result = q.push(3); // rejected
    CHECK_FALSE(result.has_value());
    CHECK(q.rejected_count() == 1);
    CHECK(q.size() == 2);
}

TEST_CASE("BackpressureQueue try_pop returns nullopt on empty", "[backpressure][queue]")
{
    BackpressureQueue<std::string> q(10);
    auto val = q.try_pop();
    CHECK_FALSE(val.has_value());
}

TEST_CASE("BackpressureQueue clear removes all items", "[backpressure][queue]")
{
    BackpressureQueue<int> q(10);
    q.push(1);
    q.push(2);
    q.clear();
    CHECK(q.empty());
    CHECK(q.size() == 0);
}

// ============================================================================
// RateLimiter tests
// ============================================================================

TEST_CASE("RateLimiter allows burst up to max", "[backpressure][ratelimit]")
{
    RateLimiter limiter(100.0, 5.0); // 100/s, burst of 5
    int acquired = 0;
    while (limiter.try_acquire())
    {
        ++acquired;
    }
    CHECK(acquired == 5);
}

TEST_CASE("RateLimiter rejects when empty", "[backpressure][ratelimit]")
{
    RateLimiter limiter(10.0, 1.0); // 10/s, burst of 1
    CHECK(limiter.try_acquire());
    CHECK_FALSE(limiter.try_acquire()); // exhausted
}

TEST_CASE("RateLimiter reset refills tokens", "[backpressure][ratelimit]")
{
    RateLimiter limiter(10.0, 3.0);
    limiter.try_acquire();
    limiter.try_acquire();
    limiter.try_acquire();
    limiter.reset();
    CHECK(limiter.available_tokens() >= 2.9); // approximately 3
}

TEST_CASE("RateLimiter try_acquire with count", "[backpressure][ratelimit]")
{
    RateLimiter limiter(100.0, 10.0);
    CHECK(limiter.try_acquire(5.0));
    CHECK(limiter.try_acquire(5.0));
    CHECK_FALSE(limiter.try_acquire(1.0)); // exhausted
}
