/// test_diff_engine.cpp — Unit tests for DiffEngine
#include "core/DiffEngine.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("DiffEngine: default construction", "[diff_engine]")
{
    DiffEngine engine;
    (void)engine;
}
TEST_CASE("DiffEngine: identical strings produce empty hunks", "[diff_engine]")
{
    DiffEngine engine;
    auto result = engine.compute_diff("hello world", "hello world");
    REQUIRE(result.hunks.empty());
}
TEST_CASE("DiffEngine: different strings produce non-empty hunks", "[diff_engine]")
{
    DiffEngine engine;
    auto result = engine.compute_diff("hello", "world");
    REQUIRE_FALSE(result.hunks.empty());
}
TEST_CASE("DiffEngine: empty to non-empty", "[diff_engine]")
{
    DiffEngine engine;
    auto result = engine.compute_diff("", "added text");
    REQUIRE_FALSE(result.hunks.empty());
}
TEST_CASE("DiffEngine: split_lines basic", "[diff_engine]")
{
    auto lines = DiffEngine::split_lines("a\nb\nc");
    REQUIRE(lines.size() == 3);
}
