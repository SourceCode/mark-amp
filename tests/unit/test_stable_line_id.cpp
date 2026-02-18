/// test_stable_line_id.cpp — Unit tests for StableLineId, StableIdAllocator, LineIdMap

#include "core/StableLineId.h"

#include <catch2/catch_test_macros.hpp>

#include <unordered_set>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// StableLineId
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("StableLineId: equality comparison", "[stable_line_id]")
{
    StableLineId a{1};
    StableLineId b{1};
    StableLineId c{2};
    REQUIRE(a == b);
    REQUIRE(a != c);
}

TEST_CASE("StableLineId: ordering", "[stable_line_id]")
{
    StableLineId a{1};
    StableLineId b{2};
    REQUIRE(a < b);
    REQUIRE_FALSE(b < a);
}

TEST_CASE("StableLineId: hashable", "[stable_line_id]")
{
    std::unordered_set<StableLineId> ids;
    ids.insert(StableLineId{1});
    ids.insert(StableLineId{2});
    ids.insert(StableLineId{1}); // duplicate
    REQUIRE(ids.size() == 2);
}

// ══════════════════════════════════════════════════════════════════════════════
// StableIdAllocator
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("StableIdAllocator: allocates unique IDs", "[stable_line_id]")
{
    StableIdAllocator alloc;
    auto id1 = alloc.allocate();
    auto id2 = alloc.allocate();
    auto id3 = alloc.allocate();
    REQUIRE(id1 != id2);
    REQUIRE(id2 != id3);
    REQUIRE(id1.id < id2.id);
}

TEST_CASE("StableIdAllocator: starts at 1", "[stable_line_id]")
{
    StableIdAllocator alloc;
    auto first = alloc.allocate();
    REQUIRE(first.id == 1);
}

TEST_CASE("StableIdAllocator: batch allocates contiguous IDs", "[stable_line_id]")
{
    StableIdAllocator alloc;
    auto batch = alloc.allocate_batch(5);
    REQUIRE(batch.size() == 5);
    for (std::size_t idx = 1; idx < batch.size(); ++idx)
    {
        REQUIRE(batch[idx].id == batch[idx - 1].id + 1);
    }
}

TEST_CASE("StableIdAllocator: current_counter tracks state", "[stable_line_id]")
{
    StableIdAllocator alloc;
    REQUIRE(alloc.current_counter() == 1);
    alloc.allocate();
    REQUIRE(alloc.current_counter() == 2);
    alloc.allocate_batch(3);
    REQUIRE(alloc.current_counter() == 5);
}

// ══════════════════════════════════════════════════════════════════════════════
// LineIdMap
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("LineIdMap: initialize creates IDs for lines", "[stable_line_id]")
{
    LineIdMap map;
    map.initialize(5);
    REQUIRE(map.size() == 5);
    for (std::size_t line = 0; line < 5; ++line)
    {
        REQUIRE(map.get(line).id != 0);
    }
}

TEST_CASE("LineIdMap: get out of range returns zero ID", "[stable_line_id]")
{
    LineIdMap map;
    map.initialize(3);
    REQUIRE(map.get(10).id == 0);
}

TEST_CASE("LineIdMap: on_insert adds new IDs", "[stable_line_id]")
{
    LineIdMap map;
    map.initialize(3);
    auto old_id0 = map.get(0);
    map.on_insert(1, 2);
    REQUIRE(map.size() == 5);
    REQUIRE(map.get(0) == old_id0); // original line 0 unchanged
}

TEST_CASE("LineIdMap: on_erase removes IDs", "[stable_line_id]")
{
    LineIdMap map;
    map.initialize(5);
    auto id4 = map.get(4);
    map.on_erase(1, 2);
    REQUIRE(map.size() == 3);
    REQUIRE(map.get(2) == id4); // line 4 shifted to position 2
}

TEST_CASE("LineIdMap: on_erase beyond bounds is safe", "[stable_line_id]")
{
    LineIdMap map;
    map.initialize(3);
    map.on_erase(10, 5); // out of range
    REQUIRE(map.size() == 3);
}
