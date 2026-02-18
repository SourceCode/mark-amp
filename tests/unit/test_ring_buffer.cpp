/// test_ring_buffer.cpp — Unit tests for RingBuffer circular buffer

#include "core/RingBuffer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("RingBuffer: default is empty", "[ring_buffer]")
{
    RingBuffer<int, 4> rb;
    REQUIRE(rb.empty());
    REQUIRE(rb.size() == 0);
    REQUIRE(rb.capacity() == 4);
    REQUIRE_FALSE(rb.has_wrapped());
}

TEST_CASE("RingBuffer: push and size", "[ring_buffer]")
{
    RingBuffer<int, 4> rb;
    rb.push(10);
    REQUIRE(rb.size() == 1);
    REQUIRE_FALSE(rb.empty());
    rb.push(20);
    REQUIRE(rb.size() == 2);
}

TEST_CASE("RingBuffer: at returns correct elements", "[ring_buffer]")
{
    RingBuffer<int, 4> rb;
    rb.push(10);
    rb.push(20);
    rb.push(30);
    REQUIRE(rb.at(0) == 10);
    REQUIRE(rb.at(1) == 20);
    REQUIRE(rb.at(2) == 30);
}

TEST_CASE("RingBuffer: back returns most recent", "[ring_buffer]")
{
    RingBuffer<int, 4> rb;
    rb.push(10);
    REQUIRE(rb.back() == 10);
    rb.push(20);
    REQUIRE(rb.back() == 20);
}

TEST_CASE("RingBuffer: wrapping overwrites oldest", "[ring_buffer]")
{
    RingBuffer<int, 4> rb;
    rb.push(1);
    rb.push(2);
    rb.push(3);
    rb.push(4);
    REQUIRE(rb.size() == 4);
    REQUIRE_FALSE(rb.has_wrapped());

    rb.push(5); // wraps, overwrites 1
    REQUIRE(rb.size() == 4);
    REQUIRE(rb.has_wrapped());
    REQUIRE(rb.at(0) == 2); // oldest is now 2
    REQUIRE(rb.back() == 5);
}

TEST_CASE("RingBuffer: total_pushes tracks all pushes", "[ring_buffer]")
{
    RingBuffer<int, 4> rb;
    REQUIRE(rb.total_pushes() == 0);
    rb.push(1);
    rb.push(2);
    rb.push(3);
    rb.push(4);
    rb.push(5);
    REQUIRE(rb.total_pushes() == 5);
}

TEST_CASE("RingBuffer: clear resets state", "[ring_buffer]")
{
    RingBuffer<int, 4> rb;
    rb.push(1);
    rb.push(2);
    rb.clear();
    REQUIRE(rb.empty());
    REQUIRE(rb.size() == 0);
}
