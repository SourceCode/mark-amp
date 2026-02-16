/// test_bounded_containers.cpp — Phase 19: BoundedDeque and BoundedString tests
///
/// Validates capacity enforcement, eviction policies, and edge cases.

#include "core/BoundedContainer.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::core;

// ══════════════════════════════════════════
// BoundedDeque — EvictOldest
// ══════════════════════════════════════════

TEST_CASE("BoundedDeque: push within capacity succeeds", "[bounded_container]")
{
    BoundedDeque<int, 5> deque;

    REQUIRE(deque.push_back(1));
    REQUIRE(deque.push_back(2));
    REQUIRE(deque.push_back(3));

    REQUIRE(deque.size() == 3);
    REQUIRE(deque.front() == 1);
    REQUIRE(deque.back() == 3);
}

TEST_CASE("BoundedDeque: evict oldest when at capacity", "[bounded_container]")
{
    BoundedDeque<int, 3> deque;

    deque.push_back(1);
    deque.push_back(2);
    deque.push_back(3);
    REQUIRE(deque.size() == 3);

    // This should evict 1
    deque.push_back(4);
    REQUIRE(deque.size() == 3);
    REQUIRE(deque.front() == 2);
    REQUIRE(deque.back() == 4);
}

TEST_CASE("BoundedDeque: evict multiple oldest", "[bounded_container]")
{
    BoundedDeque<int, 2> deque;

    deque.push_back(1);
    deque.push_back(2);
    deque.push_back(3);
    deque.push_back(4);

    REQUIRE(deque.size() == 2);
    REQUIRE(deque.front() == 3);
    REQUIRE(deque.back() == 4);
}

// ══════════════════════════════════════════
// BoundedDeque — RejectNew
// ══════════════════════════════════════════

TEST_CASE("BoundedDeque: reject new when at capacity", "[bounded_container]")
{
    BoundedDeque<int, 2> deque(EvictionPolicy::RejectNew);

    REQUIRE(deque.push_back(1));
    REQUIRE(deque.push_back(2));
    REQUIRE_FALSE(deque.push_back(3)); // rejected

    REQUIRE(deque.size() == 2);
    REQUIRE(deque.front() == 1);
    REQUIRE(deque.back() == 2);
}

// ══════════════════════════════════════════
// BoundedDeque — push_front
// ══════════════════════════════════════════

TEST_CASE("BoundedDeque: push_front evicts from back", "[bounded_container]")
{
    BoundedDeque<int, 3> deque;

    deque.push_back(1);
    deque.push_back(2);
    deque.push_back(3);

    deque.push_front(0); // should evict 3 (back)

    REQUIRE(deque.size() == 3);
    REQUIRE(deque.front() == 0);
    REQUIRE(deque.back() == 2);
}

// ══════════════════════════════════════════
// BoundedDeque — Utility
// ══════════════════════════════════════════

TEST_CASE("BoundedDeque: clear empties the container", "[bounded_container]")
{
    BoundedDeque<int, 5> deque;
    deque.push_back(1);
    deque.push_back(2);

    deque.clear();

    REQUIRE(deque.empty());
    REQUIRE(deque.size() == 0);
}

TEST_CASE("BoundedDeque: capacity is compile-time constant", "[bounded_container]")
{
    BoundedDeque<int, 100> deque;
    REQUIRE(BoundedDeque<int, 100>::capacity() == 100);
}

TEST_CASE("BoundedDeque: full() reports correctly", "[bounded_container]")
{
    BoundedDeque<int, 2> deque;
    REQUIRE_FALSE(deque.full());

    deque.push_back(1);
    REQUIRE_FALSE(deque.full());

    deque.push_back(2);
    REQUIRE(deque.full());
}

TEST_CASE("BoundedDeque: iteration works", "[bounded_container]")
{
    BoundedDeque<int, 5> deque;
    deque.push_back(10);
    deque.push_back(20);
    deque.push_back(30);

    int sum = 0;
    for (const auto& val : deque)
    {
        sum += val;
    }
    REQUIRE(sum == 60);
}

TEST_CASE("BoundedDeque: operator[] access", "[bounded_container]")
{
    BoundedDeque<std::string, 3> deque;
    deque.push_back("alpha");
    deque.push_back("beta");
    deque.push_back("gamma");

    REQUIRE(deque[0] == "alpha");
    REQUIRE(deque[1] == "beta");
    REQUIRE(deque[2] == "gamma");
}

// ══════════════════════════════════════════
// BoundedString
// ══════════════════════════════════════════

TEST_CASE("BoundedString: append within capacity", "[bounded_container]")
{
    BoundedString bstr(100);
    bstr.append("Hello ");
    bstr.append("World");

    REQUIRE(bstr.str() == "Hello World");
    REQUIRE(bstr.size() == 11);
}

TEST_CASE("BoundedString: append truncates from front at capacity", "[bounded_container]")
{
    BoundedString bstr(10);
    bstr.append("ABCDEFGHIJ"); // exactly 10 bytes
    REQUIRE(bstr.size() == 10);
    REQUIRE(bstr.str() == "ABCDEFGHIJ");

    bstr.append("KLM"); // 13 bytes total -> truncate front 3
    REQUIRE(bstr.size() == 10);
    REQUIRE(bstr.str() == "DEFGHIJKLM");
}

TEST_CASE("BoundedString: set truncates large content", "[bounded_container]")
{
    BoundedString bstr(5);
    bstr.set("Hello World"); // 11 bytes -> keep last 5

    REQUIRE(bstr.size() == 5);
    REQUIRE(bstr.str() == "World");
}

TEST_CASE("BoundedString: set within capacity", "[bounded_container]")
{
    BoundedString bstr(100);
    bstr.set("OK");

    REQUIRE(bstr.str() == "OK");
}

TEST_CASE("BoundedString: clear empties the string", "[bounded_container]")
{
    BoundedString bstr(100);
    bstr.append("content");

    bstr.clear();

    REQUIRE(bstr.empty());
    REQUIRE(bstr.size() == 0);
}

TEST_CASE("BoundedString: max_bytes returns configured limit", "[bounded_container]")
{
    BoundedString bstr(8192);
    REQUIRE(bstr.max_bytes() == 8192);
}
