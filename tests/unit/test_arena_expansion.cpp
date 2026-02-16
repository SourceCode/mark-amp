/// test_arena_expansion.cpp — FrameArenaPool expansion tests
///
/// Phase 14: Validates thread-local arena pool, PMR container usage,
/// arena reset correctness, and heap fallback on exhaustion.

#include "core/FrameArena.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <thread>
#include <vector>

TEST_CASE("FrameArenaPool: get_arena returns valid arena", "[arena]")
{
    markamp::core::FrameArenaPool pool;
    auto& arena = pool.get_arena();

    // Should be able to create a pmr vector from the arena
    auto vec = arena.make_vector<int>();
    vec.push_back(42);
    vec.push_back(99);

    REQUIRE(vec.size() == 2);
    REQUIRE(vec[0] == 42);
    REQUIRE(vec[1] == 99);
}

TEST_CASE("FrameArenaPool: same thread returns same arena", "[arena]")
{
    markamp::core::FrameArenaPool pool;
    auto& arena1 = pool.get_arena();
    auto& arena2 = pool.get_arena();

    REQUIRE(&arena1 == &arena2);
}

TEST_CASE("FrameArenaPool: different threads get independent arenas", "[arena]")
{
    markamp::core::FrameArenaPool pool;
    auto* main_arena = &pool.get_arena();

    std::uintptr_t thread_arena_addr = 0;
    std::thread worker(
        [&pool, &thread_arena_addr]()
        {
            auto& arena = pool.get_arena();
            thread_arena_addr = reinterpret_cast<std::uintptr_t>(&arena);
        });
    worker.join();

    // Thread-local arenas must be distinct
    REQUIRE(thread_arena_addr != 0);
    REQUIRE(thread_arena_addr != reinterpret_cast<std::uintptr_t>(main_arena));
}

TEST_CASE("FrameArenaPool: reset_current resets arena for reuse", "[arena]")
{
    markamp::core::FrameArenaPool pool;
    auto& arena = pool.get_arena();

    // Allocate some data
    auto vec = arena.make_vector<std::string>();
    for (int idx = 0; idx < 100; ++idx)
    {
        vec.emplace_back("test_string_" + std::to_string(idx));
    }
    REQUIRE(vec.size() == 100);

    // Reset — all arena memory is reclaimed
    pool.reset_current();

    // After reset, new allocations work from the start of the buffer
    auto vec2 = arena.make_vector<int>();
    vec2.push_back(1);
    REQUIRE(vec2.size() == 1);
}

TEST_CASE("FrameArenaPool: arena size is configurable", "[arena]")
{
    constexpr std::size_t custom_size = 128UL * 1024;
    markamp::core::FrameArenaPool pool{custom_size};
    REQUIRE(pool.arena_size() == custom_size);
}

TEST_CASE("FrameArenaPool: heap fallback on exhaustion", "[arena]")
{
    // Create a very small arena (1 KB) to force overflow
    markamp::core::FrameArenaPool pool{1024};
    auto& arena = pool.get_arena();

    // Allocate more than 1 KB — should fall back to heap, not crash
    auto vec = arena.make_vector<int>();
    vec.reserve(512); // 512 ints = 2 KB > 1 KB arena
    for (int idx = 0; idx < 512; ++idx)
    {
        vec.push_back(idx);
    }
    REQUIRE(vec.size() == 512);
    REQUIRE(vec[0] == 0);
    REQUIRE(vec[511] == 511);
}

TEST_CASE("FrameArena: pmr vector allocation", "[arena]")
{
    markamp::core::FrameArena arena;
    auto vec = arena.make_vector<double>();
    for (int idx = 0; idx < 50; ++idx)
    {
        vec.push_back(static_cast<double>(idx) * 1.5);
    }
    REQUIRE(vec.size() == 50);
    REQUIRE(vec[0] == 0.0);
    REQUIRE(vec[49] == 49.0 * 1.5);
}
