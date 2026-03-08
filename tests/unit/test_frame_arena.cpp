/// test_frame_arena.cpp — Comprehensive tests for FrameArena
#include "core/FrameArena.h"

#include <catch2/catch_test_macros.hpp>

#include <memory_resource>
#include <vector>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("FrameArena: default construction", "[frame_arena][positive]")
{
    FrameArena arena;
    (void)arena;
}

TEST_CASE("FrameArena: custom buffer size", "[frame_arena][positive]")
{
    FrameArena arena(128 * 1024);
    (void)arena;
}

TEST_CASE("FrameArena: allocator returns valid allocator", "[frame_arena][positive]")
{
    FrameArena arena;
    auto alloc = arena.allocator();
    (void)alloc;
}

TEST_CASE("FrameArena: make_vector allocates and uses arena", "[frame_arena][positive]")
{
    FrameArena arena;
    auto vec = arena.make_vector<int>();
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    REQUIRE(vec.size() == 3);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[2] == 3);
}

TEST_CASE("FrameArena: reset allows reuse", "[frame_arena][positive]")
{
    FrameArena arena(4096);
    {
        auto vec = arena.make_vector<int>();
        for (int i = 0; i < 100; ++i)
        {
            vec.push_back(i);
        }
    }
    arena.reset();
    auto vec2 = arena.make_vector<int>();
    vec2.push_back(42);
    REQUIRE(vec2.size() == 1);
}

// ── Negative Tests ──

TEST_CASE("FrameArena: kDefaultBufferSize is 64KB", "[frame_arena][negative]")
{
    REQUIRE(FrameArena::kDefaultBufferSize == 64 * 1024);
}

// ── Edge Cases ──

TEST_CASE("FrameArena: multiple resets are safe", "[frame_arena][edge]")
{
    FrameArena arena;
    arena.reset();
    arena.reset();
    auto vec = arena.make_vector<int>();
    vec.push_back(1);
    REQUIRE(vec.size() == 1);
}

TEST_CASE("FrameArenaPool: get_arena returns valid arena", "[frame_arena][edge]")
{
    FrameArenaPool pool;
    auto& arena = pool.get_arena();
    auto vec = arena.make_vector<int>();
    vec.push_back(42);
    REQUIRE(vec.size() == 1);
}
