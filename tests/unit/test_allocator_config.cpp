/// test_allocator_config.cpp — Unit tests for AllocatorConfig
#include "core/AllocatorConfig.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("AllocatorConfig: allocator_name returns valid string", "[allocator_config]")
{
    auto name = allocator_name();
    REQUIRE_FALSE(name.empty());
    // Should be either "mimalloc" or "system"
    REQUIRE((name == "mimalloc" || name == "system"));
}

TEST_CASE("AllocatorConfig: allocator_is_mimalloc is constexpr", "[allocator_config]")
{
    constexpr bool is_mi = allocator_is_mimalloc();
    (void)is_mi; // Just confirm it compiles as constexpr
}

TEST_CASE("AllocatorConfig: print_allocator_stats does not crash", "[allocator_config]")
{
    print_allocator_stats(); // Should be safe to call, no-op for system allocator
}

TEST_CASE("AllocatorConfig: allocator consistency check", "[allocator_config]")
{
    if (allocator_is_mimalloc())
    {
        REQUIRE(allocator_name() == "mimalloc");
    }
    else
    {
        REQUIRE(allocator_name() == "system");
    }
}
