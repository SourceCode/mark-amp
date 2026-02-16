/// test_lockfree_snapshot.cpp — Phase 21: SnapshotStore tests
///
/// Validates lock-free read, write serialization, and version tracking.

#include "core/SnapshotStore.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::core;

TEST_CASE("SnapshotStore: default-constructed holds default value", "[snapshot_store]")
{
    SnapshotStore<int> store;
    auto snap = store.read();
    REQUIRE(*snap == 0);
}

TEST_CASE("SnapshotStore: constructed with initial value", "[snapshot_store]")
{
    SnapshotStore<std::string> store("hello");
    auto snap = store.read();
    REQUIRE(*snap == "hello");
}

TEST_CASE("SnapshotStore: store replaces value", "[snapshot_store]")
{
    SnapshotStore<int> store(10);
    store.store(42);

    auto snap = store.read();
    REQUIRE(*snap == 42);
}

TEST_CASE("SnapshotStore: old snapshot remains valid after store", "[snapshot_store]")
{
    SnapshotStore<std::string> store("original");
    auto old_snap = store.read();

    store.store("updated");

    REQUIRE(*old_snap == "original"); // old snapshot unchanged
    REQUIRE(*store.read() == "updated");
}

TEST_CASE("SnapshotStore: update modifies value", "[snapshot_store]")
{
    SnapshotStore<std::vector<int>> store(std::vector<int>{1, 2, 3});

    store.update(
        [](const std::vector<int>& vec)
        {
            auto copy = vec;
            copy.push_back(4);
            return copy;
        });

    auto snap = store.read();
    REQUIRE(snap->size() == 4);
    REQUIRE((*snap)[3] == 4);
}

TEST_CASE("SnapshotStore: update preserves old snapshots", "[snapshot_store]")
{
    SnapshotStore<std::vector<int>> store(std::vector<int>{1, 2});
    auto before = store.read();

    store.update(
        [](const std::vector<int>& vec)
        {
            auto copy = vec;
            copy.push_back(3);
            return copy;
        });

    REQUIRE(before->size() == 2); // old snapshot intact
    REQUIRE(store.read()->size() == 3);
}

TEST_CASE("SnapshotStore: store_versioned increments version", "[snapshot_store]")
{
    SnapshotStore<int> store(0);
    REQUIRE(store.version() == 0);

    store.store_versioned(1);
    REQUIRE(store.version() == 1);

    store.store_versioned(2);
    REQUIRE(store.version() == 2);
}

TEST_CASE("SnapshotStore: plain store does not increment version", "[snapshot_store]")
{
    SnapshotStore<int> store(0);
    store.store(99);
    REQUIRE(store.version() == 0);
}

TEST_CASE("SnapshotStore: multiple sequential reads return same snapshot", "[snapshot_store]")
{
    SnapshotStore<std::string> store("stable");

    auto snap1 = store.read();
    auto snap2 = store.read();

    REQUIRE(snap1.get() == snap2.get()); // same pointer
    REQUIRE(*snap1 == "stable");
}
