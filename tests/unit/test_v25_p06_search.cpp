/// @file test_v25_p06_search.cpp
/// @brief V25 Phase 06: Search service implementation tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P06: Index files and search", "[v25][p06]")
{
    SearchServiceImpl search;
    search.index_file("test.md", "Hello world content here");
    REQUIRE(search.indexed_file_count() == 1);

    auto results = search.search("world");
    REQUIRE_FALSE(results.empty());
    REQUIRE(results[0].file_path == "test.md");
}

TEST_CASE("V25 P06: Search returns empty for no match", "[v25][p06]")
{
    SearchServiceImpl search;
    search.index_file("test.md", "Hello world");
    auto results = search.search("nonexistent");
    REQUIRE(results.empty());
}

TEST_CASE("V25 P06: Is indexed check", "[v25][p06]")
{
    SearchServiceImpl search;
    REQUIRE_FALSE(search.is_indexed());
    search.index_file("a.md", "content");
    REQUIRE(search.is_indexed());
}

TEST_CASE("V25 P06: Clear index", "[v25][p06]")
{
    SearchServiceImpl search;
    search.index_file("a.md", "content");
    search.clear_index();
    REQUIRE(search.indexed_file_count() == 0);
}
