// test_bounded_container.cpp — 10 tests for BoundedDeque and BoundedString
#include "core/BoundedContainer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("BoundedDeque push_back within capacity", "[bounded][deque]")
{
    BoundedDeque<int, 4> dq;
    CHECK(dq.push_back(1));
    CHECK(dq.push_back(2));
    CHECK(dq.push_back(3));
    CHECK(dq.size() == 3);
    CHECK(dq.front() == 1);
    CHECK(dq.back() == 3);
}

TEST_CASE("BoundedDeque evicts oldest on overflow", "[bounded][deque]")
{
    BoundedDeque<int, 3> dq;
    dq.push_back(10);
    dq.push_back(20);
    dq.push_back(30);
    CHECK(dq.full());
    dq.push_back(40); // evicts 10
    CHECK(dq.size() == 3);
    CHECK(dq.front() == 20);
    CHECK(dq.back() == 40);
}

TEST_CASE("BoundedDeque RejectNew policy", "[bounded][deque]")
{
    BoundedDeque<int, 2> dq(EvictionPolicy::RejectNew);
    CHECK(dq.push_back(1));
    CHECK(dq.push_back(2));
    CHECK_FALSE(dq.push_back(3)); // rejected
    CHECK(dq.size() == 2);
    CHECK(dq.back() == 2);
}

TEST_CASE("BoundedDeque push_front evicts from back", "[bounded][deque]")
{
    BoundedDeque<int, 2> dq;
    dq.push_front(1);
    dq.push_front(2);
    dq.push_front(3); // evicts 1 from back
    CHECK(dq.size() == 2);
    CHECK(dq.front() == 3);
    CHECK(dq.back() == 2);
}

TEST_CASE("BoundedDeque empty and clear", "[bounded][deque]")
{
    BoundedDeque<int, 5> dq;
    CHECK(dq.empty());
    dq.push_back(42);
    CHECK_FALSE(dq.empty());
    dq.clear();
    CHECK(dq.empty());
    CHECK(dq.size() == 0);
}

TEST_CASE("BoundedDeque capacity is constexpr", "[bounded][deque]")
{
    BoundedDeque<std::string, 100> dq;
    static_assert(BoundedDeque<std::string, 100>::capacity() == 100);
    CHECK(dq.capacity() == 100);
}

TEST_CASE("BoundedDeque iteration", "[bounded][deque]")
{
    BoundedDeque<int, 10> dq;
    dq.push_back(1);
    dq.push_back(2);
    dq.push_back(3);
    int sum = 0;
    for (const auto& val : dq)
    {
        sum += val;
    }
    CHECK(sum == 6);
}

TEST_CASE("BoundedString truncates from front on append", "[bounded][string]")
{
    BoundedString bs(10);
    bs.append("Hello");
    bs.append("World!"); // total 11 > 10, trims from front
    CHECK(bs.size() <= 10);
    CHECK(bs.str().find("World!") != std::string::npos);
}

TEST_CASE("BoundedString set truncates oversized input", "[bounded][string]")
{
    BoundedString bs(5);
    bs.set("ABCDEFGHIJ"); // 10 > 5, keeps last 5
    CHECK(bs.size() == 5);
    CHECK(bs.str() == "FGHIJ");
}

TEST_CASE("BoundedString clear and empty", "[bounded][string]")
{
    BoundedString bs(100);
    CHECK(bs.empty());
    bs.append("test");
    CHECK_FALSE(bs.empty());
    bs.clear();
    CHECK(bs.empty());
    CHECK(bs.max_bytes() == 100);
}
