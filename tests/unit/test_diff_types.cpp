/// test_diff_types.cpp
#include "core/DiffTypes.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("DiffOptions: default values", "[diff_types]")
{
    DiffOptions opts;
    (void)opts;
}
TEST_CASE("BlockDiffResult: default values", "[diff_types]")
{
    BlockDiffResult result;
    REQUIRE(result.diffs.empty());
    REQUIRE(result.compute_time_ms == 0);
}
