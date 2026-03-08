// test_allocator_config.cpp — 10 tests for AllocatorConfig
#include "core/AllocatorConfig.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("allocator_name returns non-empty string", "[allocator]")
{
    auto name = allocator_name();
    CHECK_FALSE(name.empty());
}

TEST_CASE("allocator_name is system or mimalloc", "[allocator]")
{
    auto name = allocator_name();
    CHECK((name == "system" || name == "mimalloc"));
}

TEST_CASE("allocator_is_mimalloc returns bool", "[allocator]")
{
    bool is_mi = allocator_is_mimalloc();
#if defined(MARKAMP_MIMALLOC_ENABLED)
    CHECK(is_mi);
#else
    CHECK_FALSE(is_mi);
#endif
}

TEST_CASE("allocator_name and is_mimalloc are consistent", "[allocator]")
{
    if (allocator_is_mimalloc())
    {
        CHECK(allocator_name() == "mimalloc");
    }
    else
    {
        CHECK(allocator_name() == "system");
    }
}

TEST_CASE("allocator_is_mimalloc is constexpr", "[allocator]")
{
    constexpr bool val = allocator_is_mimalloc();
    CHECK((val == true || val == false));
}

TEST_CASE("allocator_name is constexpr", "[allocator]")
{
    constexpr auto name = allocator_name();
    CHECK_FALSE(name.empty());
}

TEST_CASE("print_allocator_stats does not crash", "[allocator]")
{
    REQUIRE_NOTHROW(print_allocator_stats());
}

TEST_CASE("allocator_heap_usage returns value", "[allocator]")
{
    auto usage = allocator_heap_usage();
    // With system allocator, returns 0
    // With mimalloc, returns > 0
    CHECK(usage >= 0);
}

TEST_CASE("allocator_heap_usage is noexcept", "[allocator]")
{
    static_assert(noexcept(allocator_heap_usage()));
}

TEST_CASE("allocator_name is noexcept", "[allocator]")
{
    static_assert(noexcept(allocator_name()));
}
