// test_fuzzy_scorer.cpp — 10 tests for FuzzyScorer
#include "core/FuzzyScorer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("FuzzyScorer exact match returns high score", "[fuzzy]")
{
    auto result = FuzzyScorer::score("hello", "hello");
    CHECK(result.score > 0);
    CHECK(result.is_exact_match);
}

TEST_CASE("FuzzyScorer no match returns zero", "[fuzzy]")
{
    auto result = FuzzyScorer::score("xyz", "abc");
    CHECK(result.score == 0);
    CHECK(result.match_positions.empty());
}

TEST_CASE("FuzzyScorer prefix match gets prefix bonus", "[fuzzy]")
{
    auto prefix_result = FuzzyScorer::score("hel", "hello");
    auto non_prefix = FuzzyScorer::score("llo", "hello");
    CHECK(prefix_result.score > non_prefix.score);
}

TEST_CASE("FuzzyScorer camelCase matching", "[fuzzy]")
{
    auto result = FuzzyScorer::score("fdo", "Format Document");
    CHECK(result.score > 0);
    CHECK_FALSE(result.match_positions.empty());
}

TEST_CASE("FuzzyScorer word boundary bonus", "[fuzzy]")
{
    auto boundary = FuzzyScorer::score("fb", "foo_bar");
    auto non_boundary = FuzzyScorer::score("oo", "foo_bar");
    CHECK(boundary.score > 0);
    CHECK(non_boundary.score > 0);
    // Boundary match should typically score higher
    CHECK(boundary.score >= non_boundary.score);
}

TEST_CASE("FuzzyScorer empty query returns zero", "[fuzzy]")
{
    auto result = FuzzyScorer::score("", "hello");
    CHECK(result.score == 0);
}

TEST_CASE("FuzzyScorer empty candidate returns zero", "[fuzzy]")
{
    auto result = FuzzyScorer::score("hello", "");
    CHECK(result.score == 0);
}

TEST_CASE("FuzzyScorer match positions are populated", "[fuzzy]")
{
    auto result = FuzzyScorer::score("abc", "a_b_c");
    CHECK(result.score > 0);
    CHECK(result.match_positions.size() == 3);
}

TEST_CASE("FuzzyScorer consecutive chars get bonus", "[fuzzy]")
{
    auto consecutive = FuzzyScorer::score("abc", "abcdef");
    auto scattered = FuzzyScorer::score("ace", "abcdef");
    // Consecutive match should score higher than scattered
    CHECK(consecutive.score > scattered.score);
}

TEST_CASE("FuzzyScorer scoring constants are valid", "[fuzzy]")
{
    CHECK(FuzzyScorer::kConsecutiveBonus > 0);
    CHECK(FuzzyScorer::kWordBoundaryBonus > 0);
    CHECK(FuzzyScorer::kCamelCaseBonus > 0);
    CHECK(FuzzyScorer::kExactPrefixBonus > 0);
    CHECK(FuzzyScorer::kGapPenalty < 0);
    CHECK(FuzzyScorer::kUnmatchedTailPenalty < 0);
}
