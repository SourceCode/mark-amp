// test_frame_arena.cpp — 10 tests for FrameArena, FrameArenaPool, ObjectPool
#include "core/FrameArena.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// FrameArena tests
// ============================================================================

TEST_CASE("FrameArena make_vector creates usable vector", "[arena]")
{
    FrameArena arena;
    auto vec = arena.make_vector<int>();
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    CHECK(vec.size() == 3);
    CHECK(vec[0] == 1);
    CHECK(vec[2] == 3);
}

TEST_CASE("FrameArena reset invalidates previous allocations", "[arena]")
{
    FrameArena arena(1024);
    auto vec = arena.make_vector<int>();
    vec.push_back(42);
    arena.reset();
    // After reset, new allocations succeed from the same buffer
    auto vec2 = arena.make_vector<int>();
    vec2.push_back(99);
    CHECK(vec2.size() == 1);
    CHECK(vec2[0] == 99);
}

TEST_CASE("FrameArena default buffer size is 64KB", "[arena]")
{
    CHECK(FrameArena::kDefaultBufferSize == 64 * 1024);
}

// ============================================================================
// FrameArenaPool tests
// ============================================================================

TEST_CASE("FrameArenaPool provides a PoolArena", "[arena][pool]")
{
    FrameArenaPool pool;
    auto& arena = pool.get_arena();
    auto vec = arena.make_vector<int>();
    vec.push_back(10);
    CHECK(vec.size() == 1);
    CHECK(vec[0] == 10);
}

TEST_CASE("FrameArenaPool reset_current does not crash", "[arena][pool]")
{
    FrameArenaPool pool;
    auto& arena = pool.get_arena();
    auto vec = arena.make_vector<int>();
    vec.push_back(1);
    REQUIRE_NOTHROW(pool.reset_current());
}

TEST_CASE("FrameArenaPool arena_size returns configured size", "[arena][pool]")
{
    FrameArenaPool pool(512 * 1024);
    CHECK(pool.arena_size() == 512 * 1024);
}

// ============================================================================
// ObjectPool tests
// ============================================================================

TEST_CASE("ObjectPool create and destroy", "[arena][object_pool]")
{
    ObjectPool<int> pool;
    int* val = pool.create(42);
    REQUIRE(val != nullptr);
    CHECK(*val == 42);
    CHECK(pool.active_count() == 1);
    pool.destroy(val);
    CHECK(pool.active_count() == 0);
}

TEST_CASE("ObjectPool reuses freed memory", "[arena][object_pool]")
{
    ObjectPool<int, 4> pool;
    int* a = pool.create(1);
    pool.destroy(a);
    int* b = pool.create(2);
    // Should reuse the same slot
    CHECK(a == b);
    CHECK(*b == 2);
    pool.destroy(b);
}

TEST_CASE("ObjectPool grows when exhausted", "[arena][object_pool]")
{
    ObjectPool<int, 2> pool; // block of 2
    int* a = pool.create(1);
    int* b = pool.create(2);
    int* c = pool.create(3); // triggers grow
    CHECK(pool.active_count() == 3);
    CHECK(pool.total_capacity() >= 3);
    pool.destroy(a);
    pool.destroy(b);
    pool.destroy(c);
}

TEST_CASE("ObjectPool destroy nullptr is safe", "[arena][object_pool]")
{
    ObjectPool<int> pool;
    REQUIRE_NOTHROW(pool.destroy(nullptr));
    CHECK(pool.active_count() == 0);
}
