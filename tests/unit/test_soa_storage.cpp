/// test_soa_storage.cpp — Phase 23: SoAStore tests
///
/// Validates add, get, remove (swap-and-pop), column access, and iteration.

#include "core/SoAStorage.h"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <string>

using namespace markamp::core;

TEST_CASE("SoAStore: add and get", "[soa_storage]")
{
    SoAStore<int, std::string, float> store;
    store.add(1, "alpha", 1.0F);
    store.add(2, "beta", 2.0F);

    REQUIRE(store.size() == 2);
    REQUIRE(store.get<0>(0) == 1);
    REQUIRE(store.get<1>(0) == "alpha");
    REQUIRE(store.get<2>(0) == 1.0F);
    REQUIRE(store.get<0>(1) == 2);
    REQUIRE(store.get<1>(1) == "beta");
}

TEST_CASE("SoAStore: remove swap-and-pop", "[soa_storage]")
{
    SoAStore<int, std::string> store;
    store.add(10, "a");
    store.add(20, "b");
    store.add(30, "c");

    store.remove(0); // swaps [0] with [2], pops

    REQUIRE(store.size() == 2);
    REQUIRE(store.get<0>(0) == 30); // was at index 2
    REQUIRE(store.get<1>(0) == "c");
    REQUIRE(store.get<0>(1) == 20); // untouched
}

TEST_CASE("SoAStore: remove last element", "[soa_storage]")
{
    SoAStore<int> store;
    store.add(42);

    store.remove(0);

    REQUIRE(store.empty());
}

TEST_CASE("SoAStore: clear empties all columns", "[soa_storage]")
{
    SoAStore<int, float> store;
    store.add(1, 1.0F);
    store.add(2, 2.0F);

    store.clear();

    REQUIRE(store.empty());
    REQUIRE(store.size() == 0);
}

TEST_CASE("SoAStore: column access for iteration", "[soa_storage]")
{
    SoAStore<int, std::string> store;
    store.add(10, "a");
    store.add(20, "b");
    store.add(30, "c");

    const auto& ids = store.column<0>();
    REQUIRE(ids.size() == 3);

    int sum = 0;
    for (int val : ids)
    {
        sum += val;
    }
    REQUIRE(sum == 60);
}

TEST_CASE("SoAStore: get_mut modifies value in place", "[soa_storage]")
{
    SoAStore<int, std::string> store;
    store.add(1, "original");

    store.get_mut<1>(0) = "modified";

    REQUIRE(store.get<1>(0) == "modified");
}

TEST_CASE("SoAStore: reserve does not change size", "[soa_storage]")
{
    SoAStore<int> store;
    store.reserve(100);

    REQUIRE(store.size() == 0);
    REQUIRE(store.empty());
}

TEST_CASE("SoAStore: multiple types work correctly", "[soa_storage]")
{
    // Note: avoid bool — std::vector<bool> is a bitset specialization.
    // Use uint8_t as a boolean-like column instead.
    SoAStore<uint32_t, double, uint8_t, std::string> store;
    store.add(42U, 3.14, static_cast<uint8_t>(1), "test");

    REQUIRE(store.get<0>(0) == 42U);
    REQUIRE(store.get<1>(0) == 3.14);
    REQUIRE(store.get<2>(0) == 1);
    REQUIRE(store.get<3>(0) == "test");
}
