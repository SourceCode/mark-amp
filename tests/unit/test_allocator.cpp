/// test_allocator.cpp — AllocatorConfig tests
///
/// Phase 10: validates allocator selection and configuration API.

#include "core/AllocatorConfig.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>
#include <vector>

TEST_CASE("AllocatorConfig: allocator_name returns non-empty string", "[allocator]")
{
    auto name = markamp::core::allocator_name();
    REQUIRE_FALSE(name.empty());
    // In our test build (no mimalloc), should be "system"
    REQUIRE((name == "system" || name == "mimalloc"));
}

TEST_CASE("AllocatorConfig: allocator_is_mimalloc is constexpr", "[allocator]")
{
    constexpr bool is_mi = markamp::core::allocator_is_mimalloc();
#if defined(MARKAMP_MIMALLOC_ENABLED)
    REQUIRE(is_mi);
#else
    REQUIRE_FALSE(is_mi);
#endif
}

TEST_CASE("AllocatorConfig: print_allocator_stats does not crash", "[allocator]")
{
    // This should be a no-op with system allocator, callable with mimalloc
    REQUIRE_NOTHROW(markamp::core::print_allocator_stats());
}

TEST_CASE("AllocatorConfig: allocator_heap_usage returns a value", "[allocator]")
{
    auto usage = markamp::core::allocator_heap_usage();
#if defined(MARKAMP_MIMALLOC_ENABLED)
    // mimalloc should report non-zero usage
    REQUIRE(usage > 0);
#else
    // System allocator wrapper returns 0
    REQUIRE(usage == 0);
#endif
}

TEST_CASE("AllocatorConfig: allocation works with active allocator", "[allocator]")
{
    // Verify basic allocation patterns work regardless of allocator
    SECTION("vector allocation")
    {
        std::vector<int> vec(10000, 42);
        REQUIRE(vec.size() == 10000);
        REQUIRE(vec[9999] == 42);
    }

    SECTION("string allocation")
    {
        std::string str(100000, 'x');
        REQUIRE(str.size() == 100000);
    }

    SECTION("mixed allocation sizes")
    {
        // Small allocation
        auto small_ptr = std::make_unique<int>(42);
        REQUIRE(*small_ptr == 42);

        // Medium allocation (page-aligned)
        auto medium = std::vector<char>(4096, 'A');
        REQUIRE(medium.size() == 4096);

        // Large allocation (> 1MB)
        auto large = std::vector<double>(200000, 3.14);
        REQUIRE(large.size() == 200000);
        REQUIRE(large[0] == 3.14);
    }
}
