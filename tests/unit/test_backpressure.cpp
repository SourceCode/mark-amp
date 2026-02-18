/// test_backpressure.cpp — V7 Phase 19: Backpressure queue & rate limiter tests

#include "core/Backpressure.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <thread>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// BackpressureQueue — DropOldest policy
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("BackpressureQueue: push and pop", "[backpressure]")
{
    BackpressureQueue<int> queue(10);
    auto result = queue.push(42);
    REQUIRE(result.has_value());
    REQUIRE(queue.size() == 1);

    auto item = queue.try_pop();
    REQUIRE(item.has_value());
    REQUIRE(*item == 42);
    REQUIRE(queue.empty());
}

TEST_CASE("BackpressureQueue: drop-oldest on overflow", "[backpressure]")
{
    BackpressureQueue<int> queue(3, OverflowPolicy::DropOldest);
    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.push(4); // Should drop 1

    REQUIRE(queue.size() == 3);
    REQUIRE(queue.dropped_count() == 1);

    auto first = queue.try_pop();
    REQUIRE(first.has_value());
    REQUIRE(*first == 2); // 1 was dropped
}

TEST_CASE("BackpressureQueue: reject on overflow", "[backpressure]")
{
    BackpressureQueue<int> queue(2, OverflowPolicy::Reject);
    queue.push(1);
    queue.push(2);
    auto result = queue.push(3);

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::ResourceExhausted);
    REQUIRE(queue.rejected_count() == 1);
    REQUIRE(queue.size() == 2);
}

TEST_CASE("BackpressureQueue: try_pop from empty returns nullopt", "[backpressure]")
{
    BackpressureQueue<std::string> queue(5);
    auto item = queue.try_pop();
    REQUIRE_FALSE(item.has_value());
}

TEST_CASE("BackpressureQueue: capacity", "[backpressure]")
{
    BackpressureQueue<int> queue(100);
    REQUIRE(queue.capacity() == 100);
}

TEST_CASE("BackpressureQueue: clear empties queue", "[backpressure]")
{
    BackpressureQueue<int> queue(10);
    queue.push(1);
    queue.push(2);
    queue.clear();
    REQUIRE(queue.empty());
}

TEST_CASE("BackpressureQueue: preserves order (FIFO)", "[backpressure]")
{
    BackpressureQueue<int> queue(10);
    queue.push(10);
    queue.push(20);
    queue.push(30);

    REQUIRE(*queue.try_pop() == 10);
    REQUIRE(*queue.try_pop() == 20);
    REQUIRE(*queue.try_pop() == 30);
}

// ══════════════════════════════════════════════════════════════════════════════
// RateLimiter
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("RateLimiter: allows initial burst", "[backpressure]")
{
    RateLimiter limiter(10.0, 5.0); // 10 tokens/sec, burst 5
    int acquired = 0;
    for (int idx = 0; idx < 5; ++idx)
    {
        if (limiter.try_acquire())
        {
            ++acquired;
        }
    }
    REQUIRE(acquired == 5);
}

TEST_CASE("RateLimiter: rejects after burst exhausted", "[backpressure]")
{
    RateLimiter limiter(10.0, 3.0);
    limiter.try_acquire();
    limiter.try_acquire();
    limiter.try_acquire();
    REQUIRE_FALSE(limiter.try_acquire()); // Burst exhausted
}

TEST_CASE("RateLimiter: refills over time", "[backpressure]")
{
    RateLimiter limiter(1000.0, 5.0); // 1000/sec
    // Exhaust burst
    for (int idx = 0; idx < 5; ++idx)
    {
        limiter.try_acquire();
    }
    REQUIRE_FALSE(limiter.try_acquire());

    // Wait for refill
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    REQUIRE(limiter.try_acquire());
}

TEST_CASE("RateLimiter: reset restores to full", "[backpressure]")
{
    RateLimiter limiter(10.0, 5.0);
    for (int idx = 0; idx < 5; ++idx)
    {
        limiter.try_acquire();
    }
    limiter.reset();
    REQUIRE(limiter.available_tokens() >= 4.9); // Allow for small timing variance
}

TEST_CASE("RateLimiter: try_acquire with count", "[backpressure]")
{
    RateLimiter limiter(10.0, 10.0);
    REQUIRE(limiter.try_acquire(5.0));
    REQUIRE(limiter.try_acquire(5.0));
    REQUIRE_FALSE(limiter.try_acquire(1.0)); // All consumed
}
