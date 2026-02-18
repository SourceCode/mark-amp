/// test_incremental_searcher.cpp — Unit tests
#include "core/IncrementalSearcher.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("SearchMatch: default values", "[incremental_searcher]")
{
    SearchMatch match;
    REQUIRE(match.line == 0);
    REQUIRE(match.column == 0);
}

TEST_CASE("SearchConfig: has case_sensitive flag", "[incremental_searcher]")
{
    SearchConfig cfg;
    // case_sensitive defaults to true
    REQUIRE(cfg.case_sensitive);
}

TEST_CASE("IncrementalSearcher: compiles", "[incremental_searcher]")
{
    static_assert(sizeof(IncrementalSearcher) > 0);
}
