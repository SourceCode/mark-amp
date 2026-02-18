/// test_spsc_queue.cpp — Unit tests for SPSCQueue lock-free ring buffer

#include "core/SPSCQueue.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::core;

TEST_CASE("SPSCQueue: default is empty", "[spsc_queue]")
{
    SPSCQueue<int, 8> queue;
    REQUIRE(queue.empty());
    REQUIRE(queue.capacity() == 7); // power-of-2 minus 1
}

TEST_CASE("SPSCQueue: push and pop single item", "[spsc_queue]")
{
    SPSCQueue<int, 8> queue;
    REQUIRE(queue.try_push(42));
    REQUIRE_FALSE(queue.empty());

    int value = 0;
    REQUIRE(queue.try_pop(value));
    REQUIRE(value == 42);
    REQUIRE(queue.empty());
}

TEST_CASE("SPSCQueue: push until full", "[spsc_queue]")
{
    SPSCQueue<int, 4> queue; // capacity = 3
    REQUIRE(queue.try_push(1));
    REQUIRE(queue.try_push(2));
    REQUIRE(queue.try_push(3));
    REQUIRE_FALSE(queue.try_push(4)); // full
}

TEST_CASE("SPSCQueue: pop from empty returns false", "[spsc_queue]")
{
    SPSCQueue<int, 4> queue;
    int value = 0;
    REQUIRE_FALSE(queue.try_pop(value));
}

TEST_CASE("SPSCQueue: FIFO ordering", "[spsc_queue]")
{
    SPSCQueue<int, 8> queue;
    queue.try_push(10);
    queue.try_push(20);
    queue.try_push(30);

    int value = 0;
    queue.try_pop(value);
    REQUIRE(value == 10);
    queue.try_pop(value);
    REQUIRE(value == 20);
    queue.try_pop(value);
    REQUIRE(value == 30);
}

TEST_CASE("SPSCQueue: size_approx tracks items", "[spsc_queue]")
{
    SPSCQueue<int, 8> queue;
    REQUIRE(queue.size_approx() == 0);
    queue.try_push(1);
    queue.try_push(2);
    REQUIRE(queue.size_approx() == 2);
}

TEST_CASE("SPSCQueue: move semantics for push", "[spsc_queue]")
{
    SPSCQueue<std::string, 4> queue;
    std::string val = "hello";
    REQUIRE(queue.try_push(std::move(val)));

    std::string result;
    REQUIRE(queue.try_pop(result));
    REQUIRE(result == "hello");
}

TEST_CASE("SPSCQueue: wrap-around works correctly", "[spsc_queue]")
{
    SPSCQueue<int, 4> queue; // capacity = 3
    // Fill and drain, then fill again to exercise wrap-around
    queue.try_push(1);
    queue.try_push(2);
    queue.try_push(3);

    int value = 0;
    queue.try_pop(value);
    REQUIRE(value == 1);
    queue.try_pop(value);
    REQUIRE(value == 2);

    // Now push again, wrapping around the underlying array
    REQUIRE(queue.try_push(4));
    REQUIRE(queue.try_push(5));

    queue.try_pop(value);
    REQUIRE(value == 3);
    queue.try_pop(value);
    REQUIRE(value == 4);
    queue.try_pop(value);
    REQUIRE(value == 5);
}
